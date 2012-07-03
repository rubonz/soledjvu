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

#import <UIKit/UIKit.h>
#import <QuartzCore/CALayer.h>

//#import "TCPServer.h"

#include "DjVuImage.h"
#include "DjVuDocument.h"
#include "libdjvu/miniexp.h"
#include "libdjvu/ddjvuapi.h"

#include "DjVmNav.h"
#include "GPixmap.h"
#include "GBitmap.h"

static ddjvu_context_t *djvu_ctx = NULL;

static NSString *getDisplayNameForPath(NSString *path)
{
    return [[path lastPathComponent] stringByDeletingPathExtension];
}

void log_rect(NSString *str, CGRect r)
{
    NSLog(@"%@ %.f %.f %.f %.f", str, r.origin.x, r.origin.y, r.size.width, r.size.height);
}


@class Glue;
@class TCPServer;
@class DjvuRenderTask;
@class ZoomingScrollView;

@interface DjvuPageView : UIView
@property (nonatomic, retain) UIActivityIndicatorView *activityView;
@property (nonatomic, assign) CGSize renderSize;
@property (nonatomic, assign) CGSize origSize;
@end

@implementation DjvuPageView
@synthesize renderSize = _renderSize;
@synthesize origSize = _origSize;
@synthesize activityView = _activityView;

- (void)dealloc
{
    self.activityView = nil;
    [super dealloc];
}

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        self.activityView = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhiteLarge];
        [self layoutSubviews];
        [self addSubview:self.activityView];
    }
    return self;
}

- (void)layoutSubviews
{
    self.activityView.frame = self.bounds;
}
@end




@interface PixelBuffer : NSObject
{
    unsigned char *_pixbuf;
    CGContextRef _bitmapContext;
}
@property (nonatomic, assign) CGSize size;
@property (nonatomic, assign) CGSize origSize;
@end

@implementation PixelBuffer
@synthesize size = _size;
@synthesize origSize = _origSize;

void cleanup_pixbuf(void *releaseInfo, void *data)
{
    if (releaseInfo != data) {
        fprintf(stderr, "cleanup_pixbuf: releaseInfo != data\n");
    } else {
        fprintf(stderr, "cleanup_pixbuf: releaseInfo == data\n");
    }
    free(data);
    fprintf(stderr, "cleanup_pixbuf: freed\n");
}

- (void)dealloc {
    if(_bitmapContext) {
        CFRelease(_bitmapContext);
        _bitmapContext = nil;
    }   
    [super dealloc];
}

- (id)initWithSize:(CGSize)s origSize:(CGSize)origSize
{
    self = [super init];
    if (self) {
        self.size = s;
        self.origSize = s;
        _pixbuf = (unsigned char *)malloc(s.width*s.height*4);
        CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB(); 
        _bitmapContext = CGBitmapContextCreateWithData(_pixbuf,
                                                       s.width,
                                                       s.height,
                                                       8,
                                                       s.width*4,
                                                       colorSpace,
                                                       kCGImageAlphaNoneSkipLast,
                                                       cleanup_pixbuf,
                                                       _pixbuf);
        CFRelease(colorSpace);
    }
    return self;
}

- (unsigned char *)bytes { return _pixbuf; }

- (void)toView:(UIView *)v
{
    if (_bitmapContext) {
        CGImageRef cgImage = CGBitmapContextCreateImage(_bitmapContext);
        v.layer.contents = (id)cgImage;
        CFRelease(cgImage);
    }
}

@end

@interface DjvuGlue : NSObject
@end
@implementation DjvuGlue
@end



@interface DjvuDocument : NSObject
{
    ddjvu_document_t *_doc;
    miniexp_t _outline;
}
@property (nonatomic, retain) NSString *path;
@property (nonatomic, retain) NSString *title;
@end
@implementation DjvuDocument
@synthesize path = _path;
@synthesize title = _title;
- (void)dealloc
{
    if (_doc)
        ddjvu_document_release(_doc);
    _outline = NULL;
    self.path = nil;
    self.title = nil;
    [super dealloc];
}

static void handle(int wait)
{
    const ddjvu_message_t *msg;
    if (!djvu_ctx)
        return;
    if (wait)
        msg = ddjvu_message_wait(djvu_ctx);
    while ((msg = ddjvu_message_peek(djvu_ctx)))
    {
        switch(msg->m_any.tag)
        {
            case DDJVU_ERROR:
                fprintf(stderr,"djvutxt: %s\n", msg->m_error.message);
                if (msg->m_error.filename)
                    fprintf(stderr,"djvutxt: '%s:%d'\n", 
                            msg->m_error.filename, msg->m_error.lineno);
                return;
            default:
                break;
        }
        ddjvu_message_pop(djvu_ctx);
    }
}

- (id)initWithPath:(NSString *)path
{
    self = [super init];
    if (self) {
        self.path = path;
        self.title = getDisplayNameForPath(path);
        if (!djvu_ctx)
            djvu_ctx = ddjvu_context_create("soledjvu");
        if (!djvu_ctx) {
            NSLog(@"Unable to create djvu context.");
            return self;
        }
        NSLog(@"djvu cache size %lu", ddjvu_cache_get_size(djvu_ctx));
        static char pathbuf[1024];
        [path getCString:pathbuf maxLength:1024 encoding:NSUTF8StringEncoding];
        _doc = ddjvu_document_create_by_filename_utf8(djvu_ctx, pathbuf, TRUE);
        if (!_doc) {
            NSLog(@"Unable to open file '%s'", pathbuf);
            return self;
        }
        while (! ddjvu_document_decoding_done(_doc))
            handle(TRUE);
        _outline = miniexp_nil;
        while ((_outline=ddjvu_document_get_outline(_doc))==miniexp_dummy)
            handle(TRUE); 
    }

    return self;
}

- (BOOL)isLoaded
{
    return (_doc) ? YES : NO;
}

- (int)pageCount
{
    if (!_doc)
        return 0;
    return ddjvu_document_get_pagenum(_doc);
}

- (int)bookmarkCount
{
    if (!_doc)
        return 0;
    if (!miniexp_listp(_outline))
        return 0;
    return MAX(miniexp_length(_outline)-1, 0);
}

- (NSString *)bookmarkTitle:(int)index
{
    if (!_doc)
        return 0;
    if (!miniexp_listp(_outline))
        return 0;
    miniexp_t r = miniexp_nth(index+1, _outline);
    if (miniexp_listp(r)) {
        r = miniexp_car(r);
        if (miniexp_stringp(r)) {
            return [NSString stringWithCString:(const char *)miniexp_to_str(r) encoding:NSUTF8StringEncoding];
        }
    }
    return nil;
}

- (int)bookmarkPageNum:(int)index
{
    if (!_doc)
        return 0;
    if (!miniexp_listp(_outline))
        return 0;
    miniexp_t r = miniexp_nth(index+1, _outline);
    if (miniexp_listp(r)) {
        r = miniexp_nth(1, r);
        if (miniexp_stringp(r)) {
            const char *p = miniexp_to_str(r);
            int page_num = -1;
            if (*p == '#') {
                try {
                    GP<DjVuDocument> internal_doc = ddjvu_get_DjVuDocument(_doc);
                    if (internal_doc) {
                        page_num = internal_doc->id_to_page(p+1);
                        NSLog(@"doc->id_to_page '%s' page_num %d", p, page_num);
                    }
                } catch (...) {
                    page_num = strtol(p+1, 0, 10) - 1;
                    NSLog(@"strtol page_num %d", page_num);
                }
            }
            return page_num;
        }
    }
    return 0;
}

CGSize proportional_size(int w, int h, int origw, int origh)
{
    int tmp_width = w;
    int tmp_height = ((((tmp_width * origh) / origw)+7)&~7);
    if(tmp_height > h)
    {
        tmp_height = h;
        tmp_width = ((((tmp_height * origw) / origh)+7)&~7);
    }  
    return CGSizeMake(tmp_width, tmp_height);
}

- (PixelBuffer *)renderPage:(int)index maxSize:(CGSize)maxSize
{
    if (!_doc)
        return nil;
    ddjvu_page_t *page = ddjvu_page_create_by_pageno(_doc, index);
    if (!page) {
        NSLog(@"unable to decode page %d", index);
        return nil;
    }
    while (! ddjvu_page_decoding_done(page))
        handle(TRUE);
    if (ddjvu_page_decoding_error(page)) {
        handle(FALSE);
        NSLog(@"error decoding page %d", index);
        return nil;
    }

    CGSize imageSize = CGSizeMake(ddjvu_page_get_width(page), ddjvu_page_get_height(page));
    CGSize s = proportional_size(maxSize.width, maxSize.height, imageSize.width, imageSize.height);
    ddjvu_rect_t prect;
    ddjvu_rect_t rrect;
    prect.x = rrect.x = 0;
    prect.y = rrect.y = 0;
    prect.w = rrect.w = s.width;
    prect.h = rrect.h = s.height;
    
    /* Process mode specification */
    ddjvu_render_mode_t mode = DDJVU_RENDER_COLOR;
    
    /* Determine output pixel format */
    ddjvu_format_style_t style = DDJVU_FORMAT_RGBMASK32;
    unsigned int fmt_args[3];
    fmt_args[0] = 0xff;
    fmt_args[1] = 0xff00;
    fmt_args[2] = 0xff0000;
    ddjvu_format_t *fmt = ddjvu_format_create(style, 3, fmt_args);
    if (!fmt) {
        NSLog(@"unable to create format for page %d", index);
        return nil;
    }
    ddjvu_format_set_row_order(fmt, 1);
    
    /* Allocate buffer */
    int rowsize = rrect.w * 4; 

    PixelBuffer *pixelBuffer = [[[PixelBuffer alloc] initWithSize:s origSize:imageSize] autorelease];
    char *bytes = (char *)[pixelBuffer bytes];

    /* Render */
    if (! ddjvu_page_render(page, mode, &prect, &rrect, fmt, rowsize, bytes)) {
        NSLog(@"unable to render page %d %d %d %d %d %d %d", index, mode, prect.x, prect.y, prect.w, prect.h, rowsize);
        pixelBuffer = nil;
    }
    
    
    /* Free */
    ddjvu_format_release(fmt);

    ddjvu_page_release(page);
    
    return pixelBuffer;
}

/*- (NSString *)pageText:(int)page
{
    GP<DjVuImage> dimg = djvu_decode_page(_doc, page);
    if (!dimg)
        return nil;
    GP<ByteStream> text = dimg->get_text();
    if (!text)
        return nil;
    */

@end

@interface DjvuRenderTask : NSOperation
@property (nonatomic, retain) DjvuDocument *document;
@property (nonatomic, assign) int index;
@property (nonatomic, assign) CGSize maxSize;
@property (nonatomic, retain) UIView *view;
@property (nonatomic, retain) PixelBuffer *pixbuf;
@end



@implementation DjvuRenderTask
@synthesize document = _document;
@synthesize index = _index;
@synthesize maxSize = _maxSize;
@synthesize view = _view;
@synthesize pixbuf = _pixbuf;

- (void)dealloc
{
    self.pixbuf = nil;
    self.document = nil;
    self.view = nil;
    [super dealloc];
}

- (void)main
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    if (![self isCancelled]) {
        self.pixbuf = [self.document renderPage:self.index maxSize:self.maxSize];
        [self.view performSelectorOnMainThread:@selector(taskDone:) withObject:self waitUntilDone:YES];
    }
    [pool drain];
}

@end



@interface ZoomingScrollView : UIScrollView <UIScrollViewDelegate> {
    DjvuPageView        *pageView;
}
@property (nonatomic, assign) int index;
@property (nonatomic, retain) DjvuRenderTask *task;
@end

@implementation ZoomingScrollView
@synthesize index = _index;
@synthesize task = _task;

- (id)initWithFrame:(CGRect)frame
{
    if ((self = [super initWithFrame:frame])) {
        self.showsVerticalScrollIndicator = NO;
        self.showsHorizontalScrollIndicator = NO;
        self.bouncesZoom = YES;
        self.decelerationRate = UIScrollViewDecelerationRateFast;
        self.delegate = self;        
    }
    return self;
}

- (void)dealloc
{
    [self.task cancel];
    self.task = nil;
    [pageView release];
    pageView = nil;
    [super dealloc];
}

CGRect center_rect_in_size(CGRect sm, CGSize lg)
{
    // center the image as it becomes smaller than the size of the screen
    
    CGSize boundsSize = lg;
    CGRect frameToCenter = sm;
    
    // center horizontally
    if (frameToCenter.size.width < boundsSize.width)
        frameToCenter.origin.x = (boundsSize.width - frameToCenter.size.width) / 2;
    else
        frameToCenter.origin.x = 0;
    
    // center vertically
    if (frameToCenter.size.height < boundsSize.height)
        frameToCenter.origin.y = (boundsSize.height - frameToCenter.size.height) / 2;
    else
        frameToCenter.origin.y = 0;
    
    return frameToCenter;
}

- (void)layoutSubviews 
{
    [super layoutSubviews];
    
    CGSize s;
    if (!pageView.origSize.width || !pageView.origSize.height) {
        s = self.bounds.size;
    } else if (self.zoomScale > 1.0) {
        s = proportional_size(self.frame.size.width*self.zoomScale, self.frame.size.height*self.zoomScale, pageView.origSize.width, pageView.origSize.height);
    } else if (self.zooming) {
        s = pageView.frame.size;
    } else {
        s = self.frame.size;
    }
    NSLog(@"layoutSubviews: %.f %.f", s.width, s.height);
    pageView.frame = center_rect_in_size(CGRectMake(0.0, 0.0, s.width, s.height), self.frame.size);
    self.contentSize = pageView.frame.size;

    /*    if ([pageView isKindOfClass:[TilingView class]]) {
     // to handle the interaction between CATiledLayer and high resolution screens, we need to manually set the
     // tiling view's contentScaleFactor to 1.0. (If we omitted this, it would be 2.0 on high resolution screens,
     // which would cause the CATiledLayer to ask us for tiles of the wrong scales.)
     pageView.contentScaleFactor = 1.0;
     }*/
}

- (UIView *)viewForZoomingInScrollView:(UIScrollView *)scrollView
{
    return pageView;
}

- (void)scrollViewDidEndZooming:(UIScrollView *)scrollView withView:(UIView *)view atScale:(float)scale
{
    NSLog(@"scrollViewDidEndZooming:%f", scale);
    [self renderTask];
}

- (void)displayPage
{
    // clear the previous imageView
    [pageView removeFromSuperview];
    [pageView release];
    pageView = nil;
    
    self.zoomScale = 1.0;
    
    pageView = [[DjvuPageView alloc] initWithFrame:self.bounds];
    [pageView.activityView startAnimating];
    [self addSubview:pageView];
    self.contentSize = pageView.bounds.size;
    self.minimumZoomScale = 1.0;
    self.maximumZoomScale = 1.0;

    [self renderTask];
}

- (void)zoomOutPage
{
    self.zoomScale = 1.0;
    pageView.frame = self.bounds;
    self.contentSize = pageView.bounds.size;
    [self renderTask];
}

- (void)renderTask
{
    CGFloat scale = [[UIScreen mainScreen] scale];
    CGSize scaledSize = CGSizeMake(pageView.frame.size.width*scale, pageView.frame.size.height*scale);
    if ((pageView.renderSize.width == scaledSize.width) 
        && (pageView.renderSize.height == scaledSize.height))
    {
        NSLog(@"renderTask: renderSize %.f %.f == scaledSize %.f %.f", pageView.renderSize.width, pageView.renderSize.height, scaledSize.width, scaledSize.height);
        return;
    }
    DjvuDocument *document = [[self superview] document];
    if (!document) {
        NSLog(@"renderTask: no document");
        return;
    }

    [self.task cancel];
    self.task = [[[DjvuRenderTask alloc] init] autorelease];
    [self.task setDocument:document];
    [self.task setIndex:self.index];
    [self.task setMaxSize:scaledSize];
    [self.task setView:self];
    [[[TCPServer global] taskQueue] addOperation:(NSOperation *)self.task];
    NSLog(@"renderTask %d maxSize %.f %.f", self.index, scaledSize.width, scaledSize.height);
}

- (void)taskDone:(DjvuRenderTask *)task
{
    NSLog(@"taskDone page %d", task.index);
    if (self.task == task) {
        if (![task isCancelled]) {
            [pageView.activityView stopAnimating];
            if (task.pixbuf) {
                pageView.frame = center_rect_in_size(CGRectMake(0.0, 0.0, task.pixbuf.size.width, task.pixbuf.size.height), self.frame.size);
                pageView.renderSize = task.pixbuf.size;
                pageView.origSize = task.pixbuf.origSize;
                pageView.layer.contentsGravity = kCAGravityResizeAspect;
                [task.pixbuf toView:pageView];
                self.minimumZoomScale = 1.0;
                self.maximumZoomScale = 2.5;
            } else {
                UIImage *image = [Glue imageWithEmoji:[Glue unicodeForCryingFace] size:CGSizeMake(100.0, 100.0)];
                pageView.frame = center_rect_in_size(CGRectMake(0.0, 0.0, image.size.width, image.size.height), self.frame.size);
                pageView.renderSize = image.size;
                pageView.origSize = image.size;
                pageView.layer.contentsGravity = kCAGravityResizeAspect;
                pageView.layer.contents = (id)image.CGImage;
                self.minimumZoomScale = 1.0;
                self.maximumZoomScale = 1.0;
            }
        } else {
            NSLog(@"taskDone: cancelled");
        }
        self.task = nil;
    } else {
        NSLog(@"taskDone: discarding old task %p %d != %p %d", self.task, self.task.index, task, task.index);
    }
}

- (void)removeFromSuperview
{
    [self.task cancel];
    self.task = nil;
    pageView.layer.contents = nil;
    [super removeFromSuperview];
}

@end




@interface PagingScrollView : UIScrollView <UIScrollViewDelegate> 
{
    BOOL _scrollViewDidNotScrollStupidApple;
}

- (void)configurePage:(id)page forIndex:(NSUInteger)index;
- (BOOL)isDisplayingPageForIndex:(NSUInteger)index;

- (CGRect)frameForPageAtIndex:(NSUInteger)index;

- (void)tilePages;
- (id)dequeueRecycledPage;
@property (nonatomic, retain) id nuDelegate;
@property (nonatomic, retain) NSMutableSet *recycledPages;
@property (nonatomic, retain) NSMutableSet *visiblePages;
@property (nonatomic, retain) DjvuDocument *document;
@property (nonatomic, assign) CGFloat padding;
@end


@implementation PagingScrollView
@synthesize nuDelegate = _nuDelegate;
@synthesize recycledPages = _recycledPages;
@synthesize visiblePages = _visiblePages;
@synthesize document = _document;
@synthesize padding = _padding;

- (void)dealloc
{
    self.nuDelegate = nil;
    self.recycledPages = nil;
    self.visiblePages = nil;
    self.document = nil;
    [super dealloc];
}

- (id)initWithFrame:(CGRect)frame padding:(CGFloat)padding document:(DjvuDocument *)document page:(int)page
{    
    self = [super initWithFrame:[self addPadding:padding toFrame:frame]];
    if (self) {
        log_rect(@"PagingScrollView initWithFrame:", frame);
        self.padding = padding;
        self.document = document;
        self.pagingEnabled = YES;
        self.backgroundColor = [UIColor clearColor];
        self.showsVerticalScrollIndicator = NO;
        self.showsHorizontalScrollIndicator = NO;
        self.contentSize = CGSizeMake(self.bounds.size.width * [self pageCount], self.bounds.size.height);
        self.delegate = self;
        
        self.recycledPages = [[[NSMutableSet alloc] init] autorelease];
        self.visiblePages  = [[[NSMutableSet alloc] init] autorelease];
        self.contentOffset = [self getContentOffsetForPage:page];
        [self tilePages];
    }
    return self;
}

- (void)visiblePagesFrames
{
    NSLog(@"visiblePagesFrames");
    for (UIView *v in self.visiblePages) {
        log_rect(@"visiblePage", v.frame);
    }
}

- (NSUInteger)pageCount
{
    return [self.document pageCount];
}

- (void)setFrame:(CGRect)frame
{
    _scrollViewDidNotScrollStupidApple = YES;
    CGFloat xoffset = self.contentOffset.x / self.frame.size.width;
    CGRect r = [self addPadding:self.padding toFrame:frame];
    [super setFrame:r];
    self.contentOffset = CGPointMake(r.size.width*xoffset, 0);
    NSLog(@"setFrame %f %f", self.contentOffset.x, self.frame.size.width);
    self.contentSize = CGSizeMake(r.size.width * [self pageCount], r.size.height);
    [self layoutIfNeeded];
    _scrollViewDidNotScrollStupidApple = NO;
    [self tilePages];
    [self renderVisiblePages];
}

static CGRect paddingToRect(CGSize s, CGRect r)
{
    return CGRectMake(r.origin.x - s.width,
                      r.origin.y - s.height,
                      r.size.width + s.width*2.0,
                      r.size.height + s.height*2.0);
}

- (CGRect)addPadding:(CGFloat)padding toFrame:(CGRect)r
{
    return paddingToRect(CGSizeMake(padding, 0.0), r);
}

- (CGRect)frameForPageAtIndex:(NSUInteger)index {
    // We have to use our paging scroll view's bounds, not frame, to calculate the page placement. When the device is in
    // landscape orientation, the frame will still be in portrait because the pagingScrollView is the root view controller's
    // view, so its frame is in window coordinate space, which is never rotated. Its bounds, however, will be in landscape
    // because it has a rotation transform applied.
    CGRect pageFrame = self.bounds;
    pageFrame.size.width -= (2 * self.padding);
    pageFrame.origin.x = (self.bounds.size.width * index) + self.padding;
    return pageFrame;
}

- (void)layoutSubviews
{
    [super layoutSubviews];
    NSArray *arr = [self.visiblePages allObjects];
    for (ZoomingScrollView *v in arr) {
        v.frame = [self frameForPageAtIndex:v.index];
    }
}


- (UIView *)dequeueRecycledPage
{
    UIView *page = [self.recycledPages anyObject];
    if (page) {
        [[page retain] autorelease];
        [self.recycledPages removeObject:page];
    }
    return page;
}

- (BOOL)isDisplayingPageForIndex:(NSUInteger)index
{
    BOOL foundPage = NO;
    for (ZoomingScrollView *page in self.visiblePages) {
        if (page.index == index) {
            foundPage = YES;
            break;
        }
    }
    return foundPage;
}

- (void)configurePage:(ZoomingScrollView *)page forIndex:(NSUInteger)index
{
    page.index = index;
    page.frame = [self frameForPageAtIndex:index];
    
    [page displayPage];
    // Use tiled images
//    [page displayTiledImageNamed:[self imageNameAtIndex:index]
//                            size:[self imageSizeAtIndex:index]];
    
    // To use full images instead of tiled images, replace the "displayTiledImageNamed:" call
    // above by the following line:
    // [page displayImage:[self imageAtIndex:index]];
}

static NSComparisonResult compare_float(float a, float b)
{
    if (a < b)
        return NSOrderedAscending;
    if (a > b)
        return NSOrderedDescending;
    return NSOrderedSame;
}

static float render_sort_order(int index, int base_index)
{
    float val;
    val = index - base_index;
    if (val < 0.0)
        val = (val * -1.0) + 0.5;
    return val;
}

- (int)visiblePageIndex
{
    return floorf(self.contentOffset.x / self.frame.size.width);
}

- (void)tilePages:(int)before after:(int)after
{
    NSLog(@"tilePages");
    // Calculate which pages are visible
    int visiblePageIndex = [self visiblePageIndex];
    int firstNeededPageIndex = visiblePageIndex + before;
    int lastNeededPageIndex  = visiblePageIndex + after;
    firstNeededPageIndex = MAX(firstNeededPageIndex, 0);
    lastNeededPageIndex  = MIN(lastNeededPageIndex, [self pageCount] - 1);
    
    
    int lastVisiblePageIndex  = floorf((self.contentOffset.x+self.frame.size.width-1) / self.frame.size.width);
    lastVisiblePageIndex  = MIN(lastVisiblePageIndex, [self pageCount] - 1);

    
    // Recycle no-longer-visible pages 
    for (ZoomingScrollView *page in self.visiblePages) {
        if ((page.index < firstNeededPageIndex) || (page.index > lastNeededPageIndex)) {
            NSLog(@"recycling page %d (%d %d %d)", page.index, visiblePageIndex, firstNeededPageIndex, lastNeededPageIndex);
            NSLog(@"bounds %f %f", self.bounds.size.width, self.bounds.size.height);
            NSLog(@"visiblePageIndex %f %f", CGRectGetMinX(self.bounds), CGRectGetWidth(self.bounds));
            NSLog(@"contentOffset %f", self.contentOffset.x);
            [self.recycledPages addObject:page];
            [page removeFromSuperview];
        }
    }
    [self.visiblePages minusSet:self.recycledPages];
    
    NSArray *zoomOutArr = [self.visiblePages allObjects];
    
    for (ZoomingScrollView *page in zoomOutArr) {
        if ((page.index < visiblePageIndex) || (page.index > lastVisiblePageIndex)) {
            if (page.zoomScale > 1.0) {
                NSLog(@"setting zoomScale 1.0 index %d", page.index);
                [page zoomOutPage];
            }
        }
    }

    NSMutableArray *renderArr = [[[NSMutableArray alloc] init] autorelease];

    // add missing pages
    for (int index = firstNeededPageIndex; index <= lastNeededPageIndex; index++) {
        if (![self isDisplayingPageForIndex:index]) {
            [renderArr addObject:[NSNumber numberWithInt:index]];
        }
    }
    
    [renderArr sortUsingComparator:^NSComparisonResult(NSNumber *obj1, NSNumber *obj2) {
        return compare_float(render_sort_order(obj1.intValue, visiblePageIndex),
                             render_sort_order(obj2.intValue, visiblePageIndex));
    }];
    for (NSNumber *elt in renderArr) {
        UIView *page = [self dequeueRecycledPage];
        if (page == nil) {
            page = [[[ZoomingScrollView alloc] initWithFrame:CGRectZero] autorelease];
        }
        [self addSubview:page];
        [self configurePage:page forIndex:elt.intValue];
        [self.visiblePages addObject:page];
    }
}

- (void)renderVisiblePages
{
    int visiblePageIndex = [self visiblePageIndex];
    visiblePageIndex = MAX(visiblePageIndex, 0);
    NSMutableArray *renderArr = [NSMutableArray arrayWithArray:[self.visiblePages allObjects]];
    [renderArr sortUsingComparator:^NSComparisonResult(ZoomingScrollView *obj1, ZoomingScrollView *obj2) {
        return compare_float(render_sort_order(obj1.index, visiblePageIndex),
                             render_sort_order(obj2.index, visiblePageIndex));
    }];
    for (ZoomingScrollView *elt in renderArr) {
        [elt renderTask];
    }
}

- (void)tilePages
{
    [self tilePages:-2 after:2];
}

- (void)scrollViewDidScroll:(UIScrollView *)scrollView
{
    NSLog(@"scrollViewDidScroll");
    if (_scrollViewDidNotScrollStupidApple)
        return;
    [self tilePages];
    if (self.nuDelegate) {
        [[TCPServer global] eval:self.nuDelegate];
    }
}

- (int)pageIndexFromFloat:(float)val
{
    return (int)((val*(self.contentSize.width-self.frame.size.width)+self.frame.size.width/2.0)/self.frame.size.width);
}

- (CGFloat)floatOfVisiblePage
{
    CGFloat visiblePage = [self visiblePageIndex];
    CGFloat pageCount = [self pageCount] - 1;
    if (pageCount < 0.0)
        return 0.0;
    CGFloat val = visiblePage / pageCount;
    if (val < 0.0)
        val = 0.0;
    if (val > 1.0)
        val = 1.0;
    return val;
}

- (CGPoint)getContentOffsetForPage:(int)index
{
    return CGPointMake(self.frame.size.width*index, 0.0);
}

- (void)goToPage:(int)index
{
    if ((index < 0) || (index >= [self pageCount]))
        return;
    self.contentOffset = [self getContentOffsetForPage:index];
}

@end


