//
//  Glue.m
//  NuMagick
//
//  Created by arthur on 12/04/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "Nu.h"
#import "TCPServer.h"
#import <QuartzCore/QuartzCore.h>

#define M_PI   3.14159265358979323846264338327950288   
#define DEG_TO_RADIANS(angle) (angle / 180.0 * M_PI)

@class MagickGlue;

@interface Glue : NSObject
@end

@implementation Glue

//+ (int)magickConvert:(id)lst { return [MagickGlue convert:lst]; }

+ (UIWebView *)UIWebView:(CGRect)r { return [[[UIWebView alloc] initWithFrame:r] autorelease]; }

+ (void)animateWithDuration:(NSTimeInterval)duration block:(NuBlock *)block
{
    [UIView animateWithDuration:duration animations:^{[block evalWithArguments:[NSNull null] context:[[Nu sharedParser] context]];}];
}

+ (void)scaleImage:(id)lst
{
    UIImage *orig = [lst objectAtIndex:0];
    NSString *path = [lst objectAtIndex:1];
    NSNumber *pixels = [lst objectAtIndex:2];
    [Glue scaleImage:orig path:path pixels:[pixels intValue]];
}

+ (void)scaleImage:(UIImage *)orig path:(NSString *)path pixels:(int)pixels
{
    CGSize origSize = [orig size];
    if ((origSize.width <= pixels) && (origSize.height <= pixels)) {
        [Glue writeImage:orig path:path];
    } else {
        CGSize newSize = [Glue proportionalSize:[orig size] maxSize:CGSizeMake(pixels, pixels)];
        UIImage *scale = [Glue scaleImage:orig toSize:newSize];
        [Glue writeImage:scale path:path];
    }
}

+ (UIImage *)scaleImage:(UIImage *)sourceImage toSize:(CGSize)newSize
{
    CGFloat targetWidth = newSize.width;
    CGFloat targetHeight = newSize.height;
    
    CGImageRef imageRef = [sourceImage CGImage];
    CGBitmapInfo bitmapInfo = CGImageGetBitmapInfo(imageRef);
    CGColorSpaceRef colorSpaceInfo = CGImageGetColorSpace(imageRef);
    
    if (bitmapInfo == kCGImageAlphaNone) {
        bitmapInfo = kCGImageAlphaNoneSkipLast;
    }
    
    CGContextRef bitmap;
    
    if (sourceImage.imageOrientation == UIImageOrientationUp || sourceImage.imageOrientation == UIImageOrientationDown) {
        bitmap = CGBitmapContextCreate(NULL, targetWidth, targetHeight, CGImageGetBitsPerComponent(imageRef), CGImageGetBytesPerRow(imageRef), colorSpaceInfo, bitmapInfo);
        
    } else {
        bitmap = CGBitmapContextCreate(NULL, targetHeight, targetWidth, CGImageGetBitsPerComponent(imageRef), CGImageGetBytesPerRow(imageRef), colorSpaceInfo, bitmapInfo);
        
    }   
    
    if (sourceImage.imageOrientation == UIImageOrientationLeft) {
        CGContextRotateCTM (bitmap, DEG_TO_RADIANS(90));
        CGContextTranslateCTM (bitmap, 0, -targetHeight);
        
    } else if (sourceImage.imageOrientation == UIImageOrientationRight) {
        CGContextRotateCTM (bitmap, DEG_TO_RADIANS(-90));
        CGContextTranslateCTM (bitmap, -targetWidth, 0);
        
    } else if (sourceImage.imageOrientation == UIImageOrientationUp) {
        // NOTHING
    } else if (sourceImage.imageOrientation == UIImageOrientationDown) {
        CGContextTranslateCTM (bitmap, targetWidth, targetHeight);
        CGContextRotateCTM (bitmap, DEG_TO_RADIANS(-180.));
    }
    
    CGContextDrawImage(bitmap, CGRectMake(0, 0, targetWidth, targetHeight), imageRef);
    CGImageRef ref = CGBitmapContextCreateImage(bitmap);
    UIImage* newImage = [UIImage imageWithCGImage:ref];
    
    CGContextRelease(bitmap);
    CGImageRelease(ref);
    
    return newImage; 
}

+ (CGSize)proportionalSize:(CGSize)currentSize maxSize:(CGSize)maxSize
{
    int image_width = currentSize.width;
    int image_height = currentSize.height;
    int tmp_width = maxSize.width;
    int tmp_height = ((((tmp_width * image_height) / image_width)+7)&~7);
    if(tmp_height > maxSize.height)
    {
        tmp_height = maxSize.height;
        tmp_width = ((((tmp_height * image_width) / image_height)+7)&~7);
    }   
    return CGSizeMake(tmp_width, tmp_height);
}

+ (void)writeImage:(id)lst
{
    UIImage *image = [lst objectAtIndex:0];
    NSString *path = [lst objectAtIndex:1];
    [Glue writeImage:image path:path];
}

+ (void)writeImage:(UIImage *)image path:(NSString *)path
{
    [UIImagePNGRepresentation(image) writeToFile:path atomically:YES];
}

+ (void)saveToCameraRoll:(UIImage *)image
{
    if (image) {
        UIImageWriteToSavedPhotosAlbum(image, nil, nil, nil);
    }
}

+ (void)composite:(NSString *)srcbg fg:(NSString *)srcfg alpha:(CGFloat)alpha dst:(NSString *)dst
{
    UIImage *bg = [UIImage imageWithContentsOfFile:srcbg];
    UIImage *fg = [UIImage imageWithContentsOfFile:srcfg];
    UIGraphicsBeginImageContextWithOptions(bg.size, NO, bg.scale);
    CGContextRef context = UIGraphicsGetCurrentContext();
    [bg drawAtPoint:CGPointMake(0.0, 0.0)];
    [fg drawAtPoint:CGPointMake(0.0, 0.0) blendMode:kCGBlendModeHardLight alpha:alpha];
    UIImage *image = UIGraphicsGetImageFromCurrentImageContext();
    [image retain];
    UIGraphicsEndImageContext();
    [Glue writeImage:image path:dst];
}

+ (NSMutableArray *)sortFileArrayAlphabetically:(NSMutableArray *)arr
{
    NSComparator cmp = ^(NSString *a, NSString *b) {
        if ([a.lowercaseString hasPrefix:[b.lowercaseString stringByDeletingPathExtension]])
            return (NSComparisonResult)NSOrderedDescending;
        if ([b.lowercaseString hasPrefix:[a.lowercaseString stringByDeletingPathExtension]])
            return (NSComparisonResult)NSOrderedAscending;
        return (NSComparisonResult)[a localizedCaseInsensitiveCompare:b];
    };
    [arr sortUsingComparator:cmp];
    return arr;
}

static UIFont *fontWithName(NSString *fontName, NSString *str, CGSize fits)
{
    CGFloat fontSize = 12.0f;
    CGFloat val = (fits.width > fits.height) ? fits.height : fits.width;
    for(;;) {
        UIFont *f = [UIFont fontWithName:fontName size:fontSize+1.0f];
        CGSize s = [str sizeWithFont:f];
        if ((s.width > val) || (s.height > val)) {
            return f;
        }
        fontSize += 1.0f;
    }
}

+ (UIImage *)imageWithString:(NSString *)str font:(UIFont *)font
{
    CGSize size = [str sizeWithFont:font];
    UIGraphicsBeginImageContextWithOptions(size, NO, 0.0);
    /*    CGContextRef context = UIGraphicsGetCurrentContext();
     [[UIColor colorWithRed:0.875 green:0.875 blue:0.5 alpha:1.0] set];
     CGContextFillRect(context, CGRectMake(0.0, 0.0, size.width, size.height));*/
    [[UIColor whiteColor] set];
    [str drawInRect:CGRectMake(0, 0, size.width, size.height) withFont:font];
    UIImage *image = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    return image;
}

+ (UIImage *)imageWithString:(NSString *)str font:(UIFont *)font size:(CGSize)size
{
    UIGraphicsBeginImageContextWithOptions(size, NO, 0.0);
    /*    CGContextRef context = UIGraphicsGetCurrentContext();
     [[UIColor colorWithRed:0.875 green:0.875 blue:0.5 alpha:1.0] set];
     CGContextFillRect(context, CGRectMake(0.0, 0.0, size.width, size.height));*/
    CGSize textSize = [str sizeWithFont:font];
    [[UIColor whiteColor] set];
    [str drawInRect:CGRectMake((size.width-textSize.width)/2.0, (size.height-textSize.height)/2.0, textSize.width, textSize.height) withFont:font];
    UIImage *image = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    return image;
}

+ (UIImage *)imageWithEmoji:(NSString *)str size:(CGSize)size
{
    UIFont *font = fontWithName(@"AppleColorEmoji", str, size);
    return [Glue imageWithString:str font:font size:size];
}

+ (NSString *)unicodeForPileOfPoo { return @"\ue05a"; }
+ (NSString *)unicodeForCryingFace { return @"\ue411"; }

@end
