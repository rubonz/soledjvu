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

#import "TCPServer.h"
#import "Nu.h"

int main(int argc, char *argv[])
{
    @autoreleasepool {
        void (^outputcb)(NSString *str) = ^(NSString *str) {
            [[TCPServer global] output:str];
        };
        [TCPServer global];
        @try {
            return UIApplicationMain(argc, argv, nil, @"AppDelegate");
        }
        @catch (NuException* nuException) {
            outputcb([NSString stringWithFormat:@"%s", [[nuException dump] cStringUsingEncoding:NSUTF8StringEncoding]]);
        }
        @catch (id exception) {
            outputcb([NSString stringWithFormat:@"%s: %s",
                      [[exception name] cStringUsingEncoding:NSUTF8StringEncoding],
                      [[exception reason] cStringUsingEncoding:NSUTF8StringEncoding]]);
        }
    }
}