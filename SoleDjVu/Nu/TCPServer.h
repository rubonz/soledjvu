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

#import <Foundation/Foundation.h>
//#import <CoreServices/CoreServices.h>

NSString * const TCPServerErrorDomain;

typedef enum {
    kTCPServerCouldNotBindToIPv4Address = 1,
    kTCPServerCouldNotBindToIPv6Address = 2,
    kTCPServerNoSocketsAvailable = 3,
} TCPServerErrorCode;

@interface TCPServer : NSObject <NSStreamDelegate> {
    NSString *serverAddress;
    CFSocketRef ipv4socket;
    CFSocketRef ipv6socket;
    NSNetService *netService;
    NSMutableData *inputBuffer;
    NSMutableData *outputBuffer;
    NSInteger needBytes;
    SEL bytesHandler;
    NSString *commandName;
}
+ (TCPServer *)global;
- (void)parseEval:(NSString *)str;
- (void)eval:(id)progn;
- (NSString *)start;
- (void)stop;
- (void)handleNewConnectionFromAddress:(NSData *)addr inputStream:(NSInputStream *)istr outputStream:(NSOutputStream *)ostr;
- (uint32_t)wifiaddress;
- (void)sendData:(NSData *)data cmd:(NSString *)cmd;
- (void)output:(NSString *)str;
@property (nonatomic, retain) NSString *domain;
@property (nonatomic, retain) NSString *name;
@property (nonatomic, retain) NSString *type;
@property (nonatomic, assign) uint16_t port;
@property (nonatomic, retain) NSInputStream *inputStream;
@property (nonatomic, retain) NSOutputStream *outputStream;
@property (nonatomic, retain) NSOperationQueue *taskQueue;
@property (nonatomic, retain) id taskObject;
@end

char *get_rom_data(NSString *str);
