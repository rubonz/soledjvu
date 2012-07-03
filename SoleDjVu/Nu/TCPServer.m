//  SoleDjVu
//  http://soleapps.com/soleapps/soledjvu/
//
//  Copyright (c) 2012 Arthur Choung. All rights reserved.
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//

#import "TCPServer.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <arpa/inet.h>

#import "Nu.h"
#import "Glue.h"

NSString * const TCPServerErrorDomain = @"TCPServerErrorDomain";

static TCPServer *globalInstance = nil;

NSString *getPathInDocs(NSString *name)
{
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    return [[paths objectAtIndex:0] stringByAppendingPathComponent:name];
}

extern char *rom_data[];

char *get_rom_data(NSString *str)
{
    char *name = (char *) [str cStringUsingEncoding:NSUTF8StringEncoding];
    NSLog(@"get_rom_data %s", name);
    char **p;
    p = rom_data;
    while (*p) {
        if (!strcmp(*p, name)) {
            p++;
            return *p;
        }
        p++;
        p++;
    }
    return 0;
}

@implementation TCPServer

@synthesize domain = _domain;
@synthesize name = _name;
@synthesize type = _type;
@synthesize port = _port;
@synthesize inputStream = _inputStream;
@synthesize outputStream = _outputStream;
@synthesize taskQueue = _taskQueue;
@synthesize taskObject = _taskObject;

+ (TCPServer *)global
{
    if (!globalInstance) {
        globalInstance = [[super allocWithZone:NULL] init];
    }
    return globalInstance;
}

+ (id)allocWithZone:(NSZone *)zone
{
    return [self global];
}

- (void)dealloc
{
    [self stop];
    self.inputStream = nil;
    self.outputStream = nil;
    self.domain = nil;
    self.name = nil;
    self.type = nil;
    [outputBuffer release];
    outputBuffer = nil;
    [inputBuffer release];
    inputBuffer = nil;
    [super dealloc];
}

- (id)init
{
    if (globalInstance) {
        return globalInstance;
    }
    self = [super init];
    if (self) {
        NuInit();
        inputBuffer = [[NSMutableData alloc] init];
        outputBuffer = [[NSMutableData alloc] init];
        self.port = 6502;
        self.taskQueue = [[[NSOperationQueue alloc] init] autorelease];
        [self.taskQueue setMaxConcurrentOperationCount:1];
        [self loadROM:@"init"];
    }
    return self;
}

- (id)retain
{
    return self;
}

- (oneway void)release
{
}

- (NSUInteger)retainCount
{
    return NSUIntegerMax;
}

- (void)parseEval:(NSString *)str
{
    void (^outputcb)(NSString *str) = ^(NSString *str) {
        [self output:str];
    };
    [[Nu sharedParser] interact:str callback:outputcb];
}

- (void)eval:(id)progn
{
    void (^outputcb)(NSString *str) = ^(NSString *str) {
        [self output:str];
    };
    [[Nu sharedParser] interactEval:progn callback:outputcb];
}

- (void)loadROM:(NSString *)name
{
    char *data = get_rom_data(name);
    if (!data) {
        NSLog(@"unable to load rom '%@'", name);
        return;
    }
    NSLog(@"loading rom '%@'", name);
    [self parseEval:[NSString stringWithCString:data encoding:NSASCIIStringEncoding]];
    NSLog(@"loaded rom '%@'", name);
}

- (void)closeConnection
{
    [inputBuffer setLength:0];
    [commandName release];
    commandName = nil;
    if (self.inputStream) {
        [self.inputStream close];
        [self.inputStream removeFromRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        self.inputStream = nil;
    }
    if (self.outputStream) {
        [self.outputStream close];
        [self.outputStream removeFromRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        self.outputStream = nil;
    }
}

- (void)handleNewConnectionFromAddress:(NSData *)addr inputStream:(NSInputStream *)istr outputStream:(NSOutputStream *)ostr
{
    [self closeConnection];
    [[Nu sharedParser] reset];
    [inputBuffer setLength:0];
    [commandName release];
    commandName = nil;
    needBytes = 512;
    bytesHandler = @selector(parseHeader:);
    self.inputStream = istr;
    [self.inputStream setDelegate:self];
    [self.inputStream scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    [self.inputStream open];
    self.outputStream = ostr;
    [self.outputStream setDelegate:self];
    [self.outputStream scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    [self.outputStream open];
    NSLog(@"new-connection");
}

// This function is called by CFSocket when a new connection comes in.
// We gather some data here, and convert the function call to a method
// invocation on TCPServer.
static void TCPServerAcceptCallBack(CFSocketRef socket, CFSocketCallBackType type, CFDataRef address, const void *data, void *info) {
    TCPServer *server = (TCPServer *)info;
    if (kCFSocketAcceptCallBack == type) { 
        // for an AcceptCallBack, the data parameter is a pointer to a CFSocketNativeHandle
        CFSocketNativeHandle nativeSocketHandle = *(CFSocketNativeHandle *)data;
        uint8_t name[SOCK_MAXADDRLEN];
        socklen_t namelen = sizeof(name);
        NSData *peer = nil;
        if (0 == getpeername(nativeSocketHandle, (struct sockaddr *)name, &namelen)) {
            peer = [NSData dataWithBytes:name length:namelen];
        }
        CFReadStreamRef readStream = NULL;
        CFWriteStreamRef writeStream = NULL;
        CFStreamCreatePairWithSocket(kCFAllocatorDefault, nativeSocketHandle, &readStream, &writeStream);
        if (readStream && writeStream) {
            CFReadStreamSetProperty(readStream, kCFStreamPropertyShouldCloseNativeSocket, kCFBooleanTrue);
            CFWriteStreamSetProperty(writeStream, kCFStreamPropertyShouldCloseNativeSocket, kCFBooleanTrue);
            [server handleNewConnectionFromAddress:peer inputStream:(NSInputStream *)readStream outputStream:(NSOutputStream *)writeStream];
        } else {
            NSLog(@"unable to accept new connection");
            // on any failure, need to destroy the CFSocketNativeHandle 
            // since we are not going to use it any more
            close(nativeSocketHandle);
        }
        if (readStream) CFRelease(readStream);
        if (writeStream) CFRelease(writeStream);
    }
}

- (NSString *)start
{
    if (serverAddress)
        return nil;
    NSError **error = NULL;
    CFSocketContext socketCtxt = {0, self, NULL, NULL, NULL};
    ipv4socket = CFSocketCreate(kCFAllocatorDefault, PF_INET, SOCK_STREAM, IPPROTO_TCP, kCFSocketAcceptCallBack, (CFSocketCallBack)&TCPServerAcceptCallBack, &socketCtxt);
    ipv6socket = CFSocketCreate(kCFAllocatorDefault, PF_INET6, SOCK_STREAM, IPPROTO_TCP, kCFSocketAcceptCallBack, (CFSocketCallBack)&TCPServerAcceptCallBack, &socketCtxt);

    if (NULL == ipv4socket || NULL == ipv6socket) {
        if (error) *error = [[NSError alloc] initWithDomain:TCPServerErrorDomain code:kTCPServerNoSocketsAvailable userInfo:nil];
        if (ipv4socket) CFRelease(ipv4socket);
        if (ipv6socket) CFRelease(ipv6socket);
        ipv4socket = NULL;
        ipv6socket = NULL;
        return nil;
    }

    int yes = 1;
    setsockopt(CFSocketGetNative(ipv4socket), SOL_SOCKET, SO_REUSEADDR, (void *)&yes, sizeof(yes));
    setsockopt(CFSocketGetNative(ipv6socket), SOL_SOCKET, SO_REUSEADDR, (void *)&yes, sizeof(yes));

    // set up the IPv4 endpoint; if port is 0, this will cause the kernel to choose a port for us
    struct sockaddr_in addr4;
    memset(&addr4, 0, sizeof(addr4));
    addr4.sin_len = sizeof(addr4);
    addr4.sin_family = AF_INET;
    addr4.sin_port = htons(self.port);
    addr4.sin_addr.s_addr = [self wifiaddress];
    NSData *address4 = [NSData dataWithBytes:&addr4 length:sizeof(addr4)];

    if (kCFSocketSuccess != CFSocketSetAddress(ipv4socket, (CFDataRef)address4)) {
        if (error) *error = [[NSError alloc] initWithDomain:TCPServerErrorDomain code:kTCPServerCouldNotBindToIPv4Address userInfo:nil];
        if (ipv4socket) CFRelease(ipv4socket);
        if (ipv6socket) CFRelease(ipv6socket);
        ipv4socket = NULL;
        ipv6socket = NULL;
        return nil;
    }
    
    if (0 == self.port) {
        // now that the binding was successful, we get the port number 
        // -- we will need it for the v6 endpoint and for the NSNetService
        NSData *addr = [(NSData *)CFSocketCopyAddress(ipv4socket) autorelease];
        memcpy(&addr4, [addr bytes], [addr length]);
        self.port = ntohs(addr4.sin_port);
    }
    NSString *ip_addr = [NSString stringWithFormat:@"%s:%u", inet_ntoa(addr4.sin_addr), self.port];

    // set up the IPv6 endkpoint
    struct sockaddr_in6 addr6;
    memset(&addr6, 0, sizeof(addr6));
    addr6.sin6_len = sizeof(addr6);
    addr6.sin6_family = AF_INET6;
    addr6.sin6_port = htons(self.port);
    memcpy(&(addr6.sin6_addr), &in6addr_any, sizeof(addr6.sin6_addr));
    NSData *address6 = [NSData dataWithBytes:&addr6 length:sizeof(addr6)];

    if (kCFSocketSuccess != CFSocketSetAddress(ipv6socket, (CFDataRef)address6)) {
        if (error) *error = [[NSError alloc] initWithDomain:TCPServerErrorDomain code:kTCPServerCouldNotBindToIPv6Address userInfo:nil];
        if (ipv4socket) CFRelease(ipv4socket);
        if (ipv6socket) CFRelease(ipv6socket);
        ipv4socket = NULL;
        ipv6socket = NULL;
        return nil;
    }

    // set up the run loop sources for the sockets
    CFRunLoopRef cfrl = CFRunLoopGetCurrent();
    CFRunLoopSourceRef source4 = CFSocketCreateRunLoopSource(kCFAllocatorDefault, ipv4socket, 0);
    CFRunLoopAddSource(cfrl, source4, kCFRunLoopCommonModes);
    CFRelease(source4);

    CFRunLoopSourceRef source6 = CFSocketCreateRunLoopSource(kCFAllocatorDefault, ipv6socket, 0);
    CFRunLoopAddSource(cfrl, source6, kCFRunLoopCommonModes);
    CFRelease(source6);

    // we can only publish the service if we have a type to publish with
    if (nil != self.type) {
        NSString *publishingDomain = self.domain ? self.domain : @"";
        NSString *publishingName = nil;
        if (nil != self.name) {
            publishingName = self.name;
        } else {
            NSString * thisHostName = [[NSProcessInfo processInfo] hostName];
            if ([thisHostName hasSuffix:@".local"]) {
                publishingName = [thisHostName substringToIndex:([thisHostName length] - 6)];
            }
        }
        netService = [[NSNetService alloc] initWithDomain:publishingDomain type:self.type name:publishingName port:self.port];
        [netService publish];
    }

    serverAddress = [ip_addr retain];
    
    return serverAddress;
}

- (void)stop
{
    if (!serverAddress)
        return;
    [self closeConnection];
    [netService stop];
    [netService release];
    netService = nil;
    CFSocketInvalidate(ipv4socket);
    CFSocketInvalidate(ipv6socket);
    CFRelease(ipv4socket);
    CFRelease(ipv6socket);
    ipv4socket = NULL;
    ipv6socket = NULL;
    [serverAddress release];
    serverAddress = nil;
}

- (uint32_t)wifiaddress
{
    struct ifaddrs *interfaces = NULL;
    struct ifaddrs *temp_addr = NULL;
    uint32_t in_addr = htonl(INADDR_ANY);
    
    if (getifaddrs(&interfaces) == 0) {
        temp_addr = interfaces;
        while(temp_addr != NULL) {
            if(temp_addr->ifa_addr->sa_family == AF_INET) {
                NSString *ifname = [NSString stringWithUTF8String:temp_addr->ifa_name];
                if([ifname isEqualToString:@"bridge0"] || [ifname isEqualToString:@"en0"]) {
                    in_addr = ((struct sockaddr_in *)temp_addr->ifa_addr)->sin_addr.s_addr;
                }
            }
            temp_addr = temp_addr->ifa_next;
        }
    }
    freeifaddrs(interfaces);
    return in_addr;
}

- (void)stream:(NSStream *)stream handleEvent:(NSStreamEvent)eventCode
{
    switch(eventCode) {
        case NSStreamEventHasBytesAvailable: {
            uint8_t buf[1024];
            unsigned int len;
            len = [self.inputStream read:buf maxLength:1024];
            if (len) {
                NSLog(@"read %d bytes", len);
                [self input:buf length:len];
            } else {
                NSLog(@"no read buffer");
            }
            break;
        }
        case NSStreamEventHasSpaceAvailable: {
            NSLog(@"hasSpaceAvailable outputBuffer %d", [outputBuffer length]);
            if ([outputBuffer length]) {
                NSInteger n = [self.outputStream write:[outputBuffer bytes] maxLength:[outputBuffer length]];
                if (n < 0) {
                    NSLog(@"error while writing to output stream %d", [outputBuffer length]);
                } else if (n > 0) {
                    NSLog(@"wrote %d bytes", n);
                    [outputBuffer replaceBytesInRange:NSMakeRange(0, n) withBytes:NULL length:0];
                } else {
                    NSLog(@"wrote 0 bytes");
                }
            }
            break;
        }
        case NSStreamEventErrorOccurred:
            NSLog(@"stream error");
        case NSStreamEventEndEncountered:
            NSLog(@"end encountered");
            if ((stream != self.inputStream) && (stream != self.outputStream)) {
                NSLog(@"stream is not same as inputStream or outputStream");
            } else {
                if (stream == self.inputStream) {
                    NSLog(@"stream is inputStream");
                } else if (stream == self.outputStream) {
                    NSLog(@"stream is outputStream");
                }
                [self closeConnection];
            }
            break;
    }
}

- (void)input:(uint8_t *)buf length:(unsigned int)len
{
    NSLog(@"input %d bytes", len);
    [inputBuffer appendBytes:buf length:len];
    while (bytesHandler && ([inputBuffer length] >= needBytes)) {
        [self performSelectorOnMainThread:bytesHandler withObject:inputBuffer waitUntilDone:YES];
    }
}

- (void)sendOutputBuffer
{
    if (self.outputStream) {
        if ([self.outputStream hasSpaceAvailable]) {
            NSInteger n = [self.outputStream write:[outputBuffer bytes] maxLength:[outputBuffer length]];
            if (n < 0) {
                NSLog(@"error while writing to output stream %d", [outputBuffer length]);
            } else if (n > 0) {
                NSLog(@"wrote %d bytes", n);
                [outputBuffer replaceBytesInRange:NSMakeRange(0, n) withBytes:NULL length:0];
            } else {
                NSLog(@"wrote 0 bytes");
            }
        } else {
            NSLog(@"no space available for write");
            if ([self.outputStream streamError]) {
                NSLog(@"streamError");
            }
            NSLog(@"streamStatus %d", [self.outputStream streamStatus]);
        }
    }
}

- (void)sendData:(NSData *)data cmd:(NSString *)cmd
{
    NSString *str = [NSString stringWithFormat:@"%-10d%-502.502s", [data length], [cmd cStringUsingEncoding:NSASCIIStringEncoding]];
    [outputBuffer appendData:[str dataUsingEncoding:NSASCIIStringEncoding]];
    [outputBuffer appendData:data];
    [self sendOutputBuffer];
}
    
- (BOOL)sendFile:(NSString *)path
{
    NSData *data = [NSData dataWithContentsOfFile:path];
    if (!data)
        return NO;
    [self sendData:data cmd:[path lastPathComponent]];
    return YES;
}
        
- (void)output:(id)obj
{
    NSString *str = [NSString stringWithFormat:@"%@\n", [obj description]];
    NSData *data = [str dataUsingEncoding:NSUTF8StringEncoding];
    [self sendData:data cmd:@""];
}

- (void)parseHeader:(NSMutableData *)data
{
    NSString *(^trim)(NSString *str) = ^(NSString *str) {
        return [str stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];
    };
    uint8_t *bytes = [inputBuffer mutableBytes];
    needBytes = [[NSString stringWithFormat:@"%.10s", &bytes[0]] intValue];
    if (needBytes < 0) {
        NSLog(@"die");
        exit(0);
    }
    commandName = [trim([NSString stringWithFormat:@"%.502s", &bytes[10]]) retain];
    [inputBuffer replaceBytesInRange:NSMakeRange(0, 512) withBytes:NULL length:0];
    NSLog(@"parsed header: command '%@' size '%d'", commandName, needBytes);
    bytesHandler = @selector(handleCommand:);
}

- (void)handleCommand:(NSMutableData *)data
{
    uint8_t *bytes = [inputBuffer mutableBytes];
    if (needBytes > 0) {
        if ([commandName length]) {
            [self output:[NSString stringWithFormat:@"*** save '%@' %d", commandName, needBytes]];
            NSString *path = getPathInDocs(commandName);
            FILE *fp = fopen([path cStringUsingEncoding:NSUTF8StringEncoding], "w");
            if (fp) {
                size_t n = fwrite(bytes, 1, needBytes, fp);
                [self output:[NSString stringWithFormat:@"*** wrote %d bytes", n]];
                fclose(fp);
            } else {
                [self output:[NSString stringWithFormat:@"*** unable to write file '%@'", commandName]];
            }
        } else {
            NSString *str = [[[NSString alloc] initWithBytes:bytes length:needBytes encoding:NSUTF8StringEncoding] autorelease];
            [self output:@"*** parse"];
            [self parseEval:str];
        }
        [inputBuffer replaceBytesInRange:NSMakeRange(0, needBytes) withBytes:NULL length:0];
    }
    [commandName release];
    commandName = nil;
    needBytes = 512;
    bytesHandler = @selector(parseHeader:);
    [[Nu sharedParser] reset];
}

@end


@interface GlueTask : NSOperation
@property (nonatomic, assign) SEL action;
@property (nonatomic, retain) id object;
@end


@implementation GlueTask

@synthesize action = _action;
@synthesize object = _object;

- (void)dealloc
{
    self.action = nil;
    self.object = nil;
    [super dealloc];
}

- (id)initWithAction:(SEL)action object:(id)object
{
    self = [super init];
    if (self) {
        self.action = action;
        self.object = object;
    }
    return self;
}

- (void)main
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    if (![self isCancelled]) {
        [[TCPServer global] setTaskObject:self.object];
        [[Glue class] performSelector:self.action withObject:self.object];
        [[TCPServer global] setTaskObject:nil];
    }
    [pool drain];
}

@end

@interface MainTask : NSOperation
@property (nonatomic, retain) NSString *code;
@end


@implementation MainTask

@synthesize code = _code;

- (void)dealloc
{
    self.code = nil;
    [super dealloc];
}

- (id)initWithCode:(NSString *)code
{
    self = [super init];
    if (self) {
        self.code = code;
    }
    return self;
}

- (void)main
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    if (![self isCancelled]) {
        [[TCPServer global] performSelectorOnMainThread:@selector(parseEval:) withObject:self.code waitUntilDone:YES];
    }
    [pool drain];
}

@end
