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

char *rom_data[] = {

	"init", "\
(global debug-mode 0) \
(load \"boot\") \
",

	"run", "\
(load \"lib\") \
(load \"soledjvu\") \
",

	"boot", "\
(load \"nu\")\n\
\n\
(set def function)\n\
(set mac macro)\n\
(set = global)\n\
(set fn do)\n\
\n\
(mac bridge (kind name signature)\n\
     (case kind\n\
           ('constant\n\
                     `(global ,name (NuBridgedConstant constantWithName:(',name stringValue) signature:,signature)))\n\
           ('function\n\
                     `(global ,name (NuBridgedFunction functionWithName:(',name stringValue) signature:,signature)))\n\
           (else\n\
                `(NSLog \"invalid argument to bridge: '#{,kind}'\"))))\n\
\n\
\n\
(def prn (str) (if debug-mode ((TCPServer global) output:str)))\n\
(def load-rom (name) ((TCPServer global) loadROM:name))\n\
(def show-alert (title message cancel)\n\
     (((UIAlertView alloc) initWithTitle:title message:message delegate:nil cancelButtonTitle:cancel otherButtonTitles:nil) show))\n\
(def start-tcpserver ()\n\
     (if debug-mode\n\
\t (let ((addr ((TCPServer global) start)))\n\
\t   (if addr\n\
\t       (show-alert \"TCPServer\" addr \"OK\")))))\n\
(def stop-tcpserver () ((TCPServer global) stop))\n\
(def tcpserver-send-file (name) ((TCPServer global) sendFile:(path-in-cwd name)))\n\
\n\
(= app nil)\n\
(= application-did-finish-launching nil)\n\
(= application-will-resign-active nil)\n\
(= application-did-enter-background nil)\n\
(= application-will-enter-foreground nil)\n\
(= application-did-become-active nil)\n\
(= application-will-terminate nil)\n\
(= application-open-url nil)\n\
(= application-motion-began nil)\n\
(= application-motion-ended nil)\n\
(= application-motion-cancelled nil)\n\
(= application-did-receive-memory-warning nil)\n\
(= application-significant-time-change nil)\n\
\n\
(class AppDelegate is UIResponder\n\
       (- (int)application:(id) application didFinishLaunchingWithOptions:(id) launchOptions is\n\
\t  (start-tcpserver)\n\
\t  (= app application)\n\
\t  (= app-delegate self)\n\
\t  (= app-options launchOptions)\n\
\t  (= window ((UIWindow alloc) initWithFrame:((UIScreen mainScreen) bounds)))\n\
\t  (window setBackgroundColor:(UIColor blackColor))\n\
\t  (load-rom \"run\")\n\
\t  (application-did-finish-launching)\n\
\t  (window makeKeyAndVisible)\n\
\t  1)\n\
       (- (void)applicationWillResignActive:(id) application is\n\
\t  (application-will-resign-active))\n\
       (- (void)applicationDidEnterBackground:(id) application is\n\
\t  (application-did-enter-background)\n\
\t  (stop-tcpserver))\n\
       (- (void)applicationWillEnterForeground:(id) application is\n\
\t  (start-tcpserver)\n\
\t  (application-will-enter-foreground))\n\
       (- (void)applicationDidBecomeActive:(id) application is\n\
\t  (start-tcpserver)\n\
\t  (application-did-become-active))\n\
       (- (void)applicationWillTerminate:(id) application is\n\
\t  (application-will-terminate)\n\
\t  (stop-tcpserver))\n\
       (- (int)application:(id)application openURL:(id)url sourceApplication:(id)source annotation:(id)annotation is\n\
\t  (start-tcpserver)\n\
\t  (application-open-url url source annotation))\n\
       (- (void)motionBegan:(int)motion withEvent:(id)event is\n\
\t  (application-motion-began motion event))\n\
       (- (void)motionEnded:(int)motion withEvent:(id)event is\n\
\t  (application-motion-ended motion event))\n\
       (- (void)motionCancelled:(int)motion withEvent:(id)event is\n\
\t  (application-motion-cancelled motion event))\n\
       (- (void)applicationDidReceiveMemoryWarning:(id)application is\n\
\t  (application-did-receive-memory-warning))\n\
       (- (void)applicationSignificantTimeChange:(id)application is\n\
\t  (application-significant-time-change)))\n\
\n\
",

	"nu", "\
;; @file       nu.nu\n\
;; @discussion Nu library definitions. Useful extensions to common classes.\n\
;;\n\
;; @copyright  Copyright (c) 2007 Tim Burks, Radtastical Inc.\n\
;;\n\
;;   Licensed under the Apache License, Version 2.0 (the \"License\");\n\
;;   you may not use this file except in compliance with the License.\n\
;;   You may obtain a copy of the License at\n\
;;\n\
;;       http://www.apache.org/licenses/LICENSE-2.0\n\
;;\n\
;;   Unless required by applicable law or agreed to in writing, software\n\
;;   distributed under the License is distributed on an \"AS IS\" BASIS,\n\
;;   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.\n\
;;   See the License for the specific language governing permissions and\n\
;;   limitations under the License.\n\
\n\
(global rand\n\
        (do (maximum)\n\
            (let ((r (NuMath random)))\n\
                 (* maximum (- (/ r maximum) ((/ r maximum) intValue))))))\n\
\n\
(global atom?\n\
        (do (x)\n\
            (x atom)))\n\
\n\
(global char?\n\
        (do (x)\n\
            (eq (x class) ('a' class))))\n\
\n\
(global eq?\n\
        (do (x y)\n\
            (eq x y)))\n\
\n\
(global ge?\n\
        (do (x y)\n\
            (ge x y)))\n\
\n\
(global gt?\n\
        (do (x y)\n\
            (gt x y)))\n\
\n\
(global le?\n\
        (do (x y)\n\
            (le x y)))\n\
\n\
(global lt?\n\
        (do (x y)\n\
            (lt x y)))\n\
\n\
(global ne?\n\
        (do (x y)\n\
            (ne x y)))\n\
\n\
(global number?\n\
        (do (x)\n\
            (eq (x class) (1 class))))\n\
\n\
;; O(n) in the length of the list (if it is a list).\n\
(global list?\n\
        (do (x)\n\
            (or (eq x nil)\n\
                (and (pair? x)\n\
                     (list? (cdr x))))))\n\
\n\
(global null?\n\
        (do (x)\n\
            (eq x nil)))\n\
\n\
(global pair?\n\
        (do (x)\n\
            (eq (x class) NuCell)))\n\
\n\
(global string?\n\
        (do (x)\n\
            (eq (x class) (\"\" class))))\n\
\n\
(global symbol?\n\
        (do (x)\n\
            (eq (x class) NuSymbol)))\n\
\n\
(global zero?\n\
        (do (x)\n\
            (eq x 0)))\n\
\n\
;; Reverses a list.\n\
(global reverse\n\
        (do (my-list)\n\
            (if my-list\n\
                (then (append (reverse (cdr my-list)) (list (car my-list))))\n\
                (else nil))))\n\
\n\
;; Returns the first true item in the list, or nil if no item is true.\n\
(global any\n\
        (do (ls)\n\
            (ls find:(do (x) x))))\n\
\n\
;; Returns t if all elements of the list are true.\n\
(global all\n\
        (do (ls)\n\
            (not (any (ls map:(do (x) (not x)))))))\n\
\n\
;; Applies a function to a list of arguments.\n\
;; For example (apply + '(1 2)) returns 3.\n\
(global apply\n\
        (macro _ (fn *fnargs)\n\
             `(eval (cons ,fn ,*fnargs))))\n\
\n\
;; Evaluates an expression and raises a NuAssertionFailure if the result is false.\n\
;; For example (assert (eq 1 1)) does nothing but (assert (eq (+ 1 1) 1)) throws\n\
;; an exception.\n\
(global assert\n\
        (macro _ (*body)\n\
             `(progn\n\
                    (set expression ,(car *body))\n\
                    (if (not (eval expression))\n\
                        (then (throw ((NSException alloc)\n\
                                      initWithName:\"NuAssertionFailure\"\n\
                                      reason:(expression stringValue)\n\
                                      userInfo:nil)))))))\n\
\n\
;; Allows mapping a function over multiple lists.\n\
;; For example (map + '(1 2) '(3 4)) returns '(4 6).\n\
;; The length of the result is the same as that of the shortest list passed in.\n\
;; For example (map + '(1 2) '(3)) returns '(4).\n\
(global map\n\
        (progn\n\
              (set _map\n\
                   (do (f _lists)\n\
                       (if (_lists select:(do (x) (not x)))\n\
                           (then '())\n\
                           (else\n\
                                (cons\n\
                                     (apply f (_lists map: (do (ls) (first ls))))\n\
                                     (_map f (_lists map: (do (ls) (rest ls)))))))))\n\
              (do (fun *lists)\n\
                  (_map fun *lists))))\n\
\n\
;; Sorts a list.\n\
(global sort\n\
        (do (ls *more-args)\n\
            (set block (if *more-args\n\
                           (then (first *more-args))\n\
                           (else (do (a b) (a compare:b)))))\n\
            (((apply array ls) sortedArrayUsingBlock:block) list)))\n\
\n\
(if (or (eq (uname) \"Darwin\") (eq (uname \"iOS\"))) ;; throw is currently only available with the Darwin runtime\n\
    (then\n\
         ;; Evaluates an expression and raises a NuAssertionFailure if the result is false.\n\
         ;; For example (assert (eq 1 1)) does nothing but (assert (eq (+ 1 1) 1)) throws\n\
         ;; an exception.\n\
         (global assert\n\
                 (macro _ (*body)\n\
                      `(progn\n\
                             (set expression ,(car *body))\n\
                             (if (not (eval expression))\n\
                                 (then (throw ((NSException alloc)\n\
                                               initWithName:\"NuAssertionFailure\"\n\
                                               reason:(expression stringValue)\n\
                                               userInfo:nil)))))))\n\
         \n\
         ;; Throws an exception.\n\
         ;; This function is more concise and easier to remember than throw.\n\
         (global throw*\n\
                 (do (type reason)\n\
                     (throw ((NSException alloc) initWithName:type\n\
                             reason:reason\n\
                             userInfo:nil)))))\n\
    (else\n\
         (global assert (macro _ () (NSLog \"warning: assert is unavailable\")))\n\
         (global throw* (macro _ () (NSLog \"warning: throw* is unavailable\")))\n\
         (global throw  (macro _ () (NSLog \"warning: throw is unavailable\")))))\n\
\n\
\n\
;; Returns an array of filenames matching a given pattern.\n\
;; the pattern is a string that is converted into a regular expression.\n\
(global filelist\n\
        (do (pattern)\n\
            (let ((r (regex pattern))\n\
                  (results ((NSMutableSet alloc) init))\n\
                  (enumerator ((NSFileManager defaultManager) enumeratorAtPath:\".\"))\n\
                  (filename nil))\n\
                 (while (set filename (enumerator nextObject))\n\
                        (if (r findInString:(filename stringValue))\n\
                            (results addObject:filename)))\n\
                 ((results allObjects) sortedArrayUsingSelector:\"compare:\"))))\n\
\n\
(if (eq (uname) \"Darwin\")\n\
    (class NuCell\n\
         ;; Convert a list into an NSRect. The list must have at least four elements.\n\
         (- (NSRect) rectValue is (list (self first) (self second) (self third) (self fourth)))\n\
         ;; Convert a list into an NSPoint.  The list must have at least two elements.\n\
         (- (NSPoint) pointValue is (list (self first) (self second)))\n\
         ;; Convert a list into an NSSize.  The list must have at least two elements.\n\
         (- (NSSize) sizeValue is (list (self first) (self second)))\n\
         ;; Convert a list into an NSRange.  The list must have at least two elements.\n\
         (- (NSRange) rangeValue is (list (self first) (self second)))))\n\
\n\
;; profiling macro - experimental\n\
(global profile\n\
        (macro _ (name *body)\n\
             `(progn ((NuProfiler defaultProfiler) start:,name)\n\
                     (set __result (progn ,@*body))\n\
                     ((NuProfiler defaultProfiler) stop)\n\
                     __result)))\n\
\n\
;; import some useful C functions\n\
(global random  (NuBridgedFunction functionWithName:\"random\" signature:\"l\"))\n\
(global srandom (NuBridgedFunction functionWithName:\"srandom\" signature:\"vI\"))\n\
\n\
",

	"lib", "\
(if nil\n\
    (bridge constant AVCaptureSessionPresetPhoto \"@\")\n\
  (bridge constant AVCaptureSessionPresetHigh \"@\")\n\
  (bridge constant AVCaptureSessionPresetMedium \"@\")\n\
  (bridge constant AVCaptureSessionPresetLow \"@\")\n\
  (bridge constant AVCaptureSessionPreset352x288 \"@\")\n\
  (bridge constant AVCaptureSessionPreset640x480 \"@\")\n\
  (bridge constant AVCaptureSessionPresetiFrame960x540 \"@\")\n\
  (bridge constant AVCaptureSessionPreset1280x720 \"@\")\n\
  (bridge constant AVCaptureSessionPresetiFrame1280x720 \"@\")\n\
\n\
  (bridge constant AVMediaTypeVideo \"@\")\n\
  (bridge constant AVMediaTypeAudio \"@\")\n\
  (bridge constant AVMediaTypeText \"@\")\n\
  (bridge constant AVMediaTypeClosedCaption \"@\")\n\
  (bridge constant AVMediaTypeSubtitle \"@\")\n\
  (bridge constant AVMediaTypeTimecode \"@\")\n\
  (bridge constant AVMediaTypeTimedMetadata \"@\")\n\
  (bridge constant AVMediaTypeMuxed \"@\")\n\
\n\
  (bridge constant AVLayerVideoGravityResize \"@\")\n\
  (bridge constant AVLayerVideoGravityResizeAspect \"@\")\n\
  (bridge constant AVLayerVideoGravityResizeAspectFill \"@\"))\n\
\n\
(= UITableViewStylePlain 0)\n\
(= UITableViewStyleGrouped 1)\n\
\n\
(= UITableViewCellStyleDefault 0)\n\
(= UITableViewCellStyleValue1 1)\n\
(= UITableViewCellStyleValue2 2)\n\
(= UITableViewCellStyleSubtitle 3)\n\
\n\
(= UIBarButtonItemStylePlain 0)\n\
(= UIBarButtonItemStyleBordered 1)\n\
(= UIBarButtonItemStyleDone 2)\n\
\n\
(= UIViewAutoresizingFlexibleLeftMargin 1)\n\
(= UIViewAutoresizingFlexibleWidth 2)\n\
(= UIViewAutoresizingFlexibleRightMargin 4)\n\
(= UIViewAutoresizingFlexibleTopMargin 8)\n\
(= UIViewAutoresizingFlexibleHeight 16)\n\
(= UIViewAutoresizingFlexibleBottomMargin 32)\n\
\n\
(= UIModalPresentationFullScreen 0)\n\
(= UIModalPresentationPageSheet 1)\n\
(= UIModalPresentationFormSheet 2)\n\
(= UIModalPresentationCurrentContext 3)\n\
\n\
(= UIModalTransitionStyleCoverVertical 0)\n\
(= UIModalTransitionStyleFlipHorizontal 1)\n\
(= UIModalTransitionStyleCrossDissolve 2)\n\
(= UIModalTransitionStylePartialCurl 3)\n\
\n\
(= UITextBorderStyleNone 0)\n\
(= UITextBorderStyleLine 1)\n\
(= UITextBorderStyleBezel 2)\n\
(= UITextBorderStyleRoundedRect 3)\n\
\n\
(= UITextFieldViewModeNever 0)\n\
(= UITextFieldViewModeWhileEditing 1)\n\
(= UITextFieldViewModeUnlessEditing 2)\n\
(= UITextFieldViewModeAlways 3)\n\
\n\
(mac new (a) `((,a alloc) init))\n\
\n\
(def point-x (p) (nth 0 p))\n\
(def point-y (p) (nth 1 p))\n\
(def size-w (s) (nth 0 s))\n\
(def size-h (s) (nth 1 s))\n\
(def size-r (s) (list 0 0 (size-w s) (size-h s)))\n\
(def rect-x (r) (nth 0 r))\n\
(def rect-y (r) (nth 1 r))\n\
(def rect-w (r) (nth 2 r))\n\
(def rect-h (r) (nth 3 r))\n\
(def rect-s (r) (list (rect-w r) (rect-h r)))\n\
(def view-w (v) (rect-w (v frame)))\n\
(def view-h (v) (rect-h (v frame)))\n\
(mac self-view-w () `(view-w (self view)))\n\
(mac self-view-h () `(view-h (self view)))\n\
\n\
(= file-manager (NSFileManager defaultManager))\n\
\n\
(bridge constant NSFileTypeRegular \"@\")\n\
(= NSDocumentDirectory 9)\n\
(= NSUserDomainMask 1)\n\
(bridge function NSSearchPathForDirectoriesInDomains \"@iii\")\n\
\n\
(def docs-path ()\n\
     (let ((paths (NSSearchPathForDirectoriesInDomains NSDocumentDirectory NSUserDomainMask 1)))\n\
       (paths objectAtIndex:0)))\n\
\n\
(def path-in-docs (path)\n\
     ((docs-path) stringByAppendingPathComponent:path))\n\
\n\
(def new-regex (pattern)\n\
     (NSRegularExpression regularExpressionWithPattern:pattern options:nil error:nil))\n\
\n\
(def regex-match (re str)\n\
     (let ((val (re rangeOfFirstMatchInString:str options:nil range:(list 0 (str length)))))\n\
       (if (eq (car val) NSNotFound) nil (else t))))\n\
\n\
(def pattern-match (pattern str)\n\
     (let ((re (new-regex pattern)))\n\
       (regex-match re str)))\n\
\n\
(def regex-files-in-path (re path)\n\
     (let ((enum (file-manager enumeratorAtPath:path))\n\
\t   (arr ((NSMutableArray alloc) init)))\n\
       (while (set file (enum nextObject))\n\
\t (if (eq ((enum fileAttributes) fileType) NSFileTypeRegular)\n\
\t     (if (or (not re)\n\
\t\t     (regex-match re file)) \n\
\t\t (arr addObject:file))))\n\
       arr))\n\
(def all-files-in-path (path) (regex-files-in-path nil path))\n\
(def all-files-in-docs () (all-files-in-path (docs-path)))\n\
(def match-files-in-docs (pattern) (regex-files-in-path (new-regex pattern) (docs-path)))\n\
\n\
(bridge constant UIImagePickerControllerMediaType \"@\")\n\
(bridge constant UIImagePickerControllerOriginalImage \"@\")\n\
(bridge constant UIImagePickerControllerEditedImage \"@\")\n\
(bridge constant UIImagePickerControllerCropRect \"@\")\n\
(bridge constant UIImagePickerControllerMediaURL \"@\")\n\
(bridge constant UIImagePickerControllerReferenceURL \"@\")\n\
(bridge constant UIImagePickerControllerMediaMetadata \"@\")\n\
\n\
(bridge constant kUTTypeImage \"@\")\n\
(bridge constant kUTTypeJPEG \"@\")\n\
(bridge constant kUTTypeJPEG2000 \"@\")\n\
(bridge constant kUTTypeTIFF \"@\")\n\
(bridge constant kUTTypePICT \"@\")\n\
(bridge constant kUTTypeGIF \"@\")\n\
(bridge constant kUTTypePNG \"@\")\n\
(bridge constant kUTTypeQuickTimeImage \"@\")\n\
(bridge constant kUTTypeAppleICNS \"@\")\n\
(bridge constant kUTTypeBMP \"@\")\n\
(bridge constant kUTTypeICO \"@\")\n\
\n\
(= UIControlStateNormal 0)\n\
(= UIControlStateHighlighted 1)\n\
(= UIControlStateDisabled 2)\n\
(= UIControlStateSelected 4)\n\
\n\
(= UIViewContentModeScaleToFill 0)\n\
(= UIViewContentModeScaleAspectFit 1)\n\
(= UIViewContentModeScaleAspectFill 2)\n\
(= UIViewContentModeRedraw 3)\n\
(= UIViewContentModeCenter 4)\n\
(= UIViewContentModeTop 5)\n\
(= UIViewContentModeBottom 6)\n\
(= UIViewContentModeLeft 7)\n\
(= UIViewContentModeRight 8)\n\
(= UIViewContentModeTopLeft 9)\n\
(= UIViewContentModeTopRight 10)\n\
(= UIViewContentModeBottomLeft 11)\n\
(= UIViewContentModeBottomRight 12)\n\
\n\
(= UIPopoverArrowDirectionUp 1)\n\
(= UIPopoverArrowDirectionDown 2)\n\
(= UIPopoverArrowDirectionLeft 4)\n\
(= UIPopoverArrowDirectionRight 8)\n\
(= UIPopoverArrowDirectionAny 15)\n\
\n\
(= UIImagePickerControllerSourceTypePhotoLibrary 0)\n\
(= UIImagePickerControllerSourceTypeCamera 1)\n\
(= UIImagePickerControllerSourceTypeSavedPhotosAlbum 2)\n\
\n\
(def bundle-path () ((NSBundle mainBundle) resourcePath))\n\
\n\
(def path-in-bundle (name) ((bundle-path) stringByAppendingPathComponent:name))\n\
\n\
(def app-frame ()\n\
     (zero-origin ((UIScreen mainScreen) applicationFrame)))\n\
\n\
(mac autoresizing-mask (*a)\n\
     (if *a\n\
\t (|\n\
\t  (case (car *a)\n\
\t\t('w UIViewAutoresizingFlexibleWidth)\n\
\t\t('h UIViewAutoresizingFlexibleHeight)\n\
\t\t('l UIViewAutoresizingFlexibleLeftMargin)\n\
\t\t('r UIViewAutoresizingFlexibleRightMargin)\n\
\t\t('t UIViewAutoresizingFlexibleTopMargin)\n\
\t\t('b UIViewAutoresizingFlexibleBottomMargin)\n\
\t\t(else 0))\n\
\t  (apply autoresizing-mask (cdr *a)))\n\
       (else 0)))\n\
\n\
(mac set-autoresizing-mask (v *a)\n\
     `(,v setAutoresizingMask:(autoresizing-mask ,@*a)))\n\
\n\
(mac new-w-frame (a b) `((,a alloc) initWithFrame:,b))\n\
\n\
(= cwd (docs-path))\n\
(def path-in-cwd (path) (cwd stringByAppendingPathComponent:path))\n\
(def contents-of-path (path)\n\
     (file-manager contentsOfDirectoryAtPath:path error:nil))\n\
(def ls-path-f (f path)\n\
     (let ((arr (f path)))\n\
       (for ((set i 0) (< i (arr count)) (set i (+ i 1)))\n\
\t    (let ((elt (arr objectAtIndex:i)))\n\
\t      (prn \"#{elt} #{(file-size elt)}\")))))\n\
(def ls-f (f lst)\n\
     (if lst\n\
\t (map (fn (elt) (ls-path-f f (path-in-docs elt))) lst)\n\
       (else (ls-path-f f cwd))))\n\
(def lsl (*a) (ls-f all-files-in-path *a))\n\
(def ls (*a) (ls-f contents-of-path *a))\n\
\n\
(def rm (name) (file-manager removeItemAtPath:(path-in-cwd name) error:nil))\n\
(def rm/ (path) (file-manager removeItemAtPath:path error:nil))\n\
\n\
(def cp (src dst) (file-manager copyItemAtPath:(path-in-cwd src) toPath:(path-in-cwd dst) error:nil))\n\
(def mv (src dst) (file-manager moveItemAtPath:(path-in-cwd src) toPath:(path-in-cwd dst) error:nil))\n\
(def mv-to-dir (src dst)\n\
     (let ((fulldst ((path-in-cwd dst) stringByAppendingPathComponent:src)))\n\
       (file-manager moveItemAtPath:(path-in-cwd src) toPath:fulldst error:nil)))\n\
\n\
(def mkdir (name) (file-manager createDirectoryAtPath:(path-in-cwd name) withIntermediateDirectories:1 attributes:nil error:nil))\n\
(def file-exists (path) (file-manager fileExistsAtPath:path))\n\
(def resolve-file (name)\n\
     (if (file-exists (path-in-cwd name))\n\
\t (path-in-cwd name)\n\
       (else (path-in-bundle name))))\n\
(def resolve-bundle (name)\n\
     (if (file-exists (path-in-bundle name))\n\
\t (path-in-bundle name)\n\
       (else (path-in-docs name))))\n\
(def image-from-file (path) (UIImage imageWithContentsOfFile:path))\n\
(def file-size (name)\n\
     (let ((path (path-in-cwd name)))\n\
       (if (file-exists path)\n\
\t   (let ((dict (file-manager attributesOfItemAtPath:path error:nil)))\n\
\t     (dict fileSize))\n\
\t (else 0))))\n\
\n\
(= UIButtonTypeCustom 0)\n\
(= UIButtonTypeRoundedRect 1)\n\
(= UIButtonTypeDetailDisclosure 2)\n\
(= UIButtonTypeInfoLight 3)\n\
(= UIButtonTypeInfoDark 4)\n\
(= UIButtonTypeContactAdd 5)\n\
\n\
(= UIControlEventTouchDown 1)\n\
(= UIControlEventTouchDownRepeat 2)\n\
(= UIControlEventTouchDragInside 4)\n\
(= UIControlEventTouchDragOutside 8)\n\
(= UIControlEventTouchDragEnter 16)\n\
(= UIControlEventTouchDragExit 32)\n\
(= UIControlEventTouchUpInside 64)\n\
(= UIControlEventTouchUpOutside 128)\n\
(= UIControlEventTouchCancel 256)\n\
(= UIControlEventValueChanged 4096)\n\
(= UIControlEventEditingDidBegin 65536)\n\
(= UIControlEventEditingChanged 131072)\n\
(= UIControlEventEditingDidEnd 262144)\n\
(= UIControlEventEditingDidEndOnExit 524288)\n\
\n\
(= UIDeviceOrientationUnknown 0)\n\
(= UIDeviceOrientationPortrait 1)\n\
(= UIDeviceOrientationPortraitUpsideDown 2)\n\
(= UIDeviceOrientationLandscapeLeft 3)\n\
(= UIDeviceOrientationLandscapeRight 4)\n\
(= UIDeviceOrientationFaceUp 5)\n\
(= UIDeviceOrientationFaceDown 6)\n\
\n\
(= UIInterfaceOrientationPortrait UIDeviceOrientationPortrait)\n\
(= UIInterfaceOrientationPortraitUpsideDown UIDeviceOrientationPortraitUpsideDown)\n\
(= UIInterfaceOrientationLandscapeLeft UIDeviceOrientationLandscapeRight)\n\
(= UIInterfaceOrientationLandscapeRight UIDeviceOrientationLandscapeLeft)\n\
\n\
(= UIBarButtonSystemItemDone 0)\n\
(= UIBarButtonSystemItemCancel 1)\n\
(= UIBarButtonSystemItemEdit 2)\n\
(= UIBarButtonSystemItemSave 3)\n\
(= UIBarButtonSystemItemAdd 4)\n\
(= UIBarButtonSystemItemFlexibleSpace 5)\n\
(= UIBarButtonSystemItemFixedSpace 6)\n\
(= UIBarButtonSystemItemCompose 7)\n\
(= UIBarButtonSystemItemReply 8)\n\
(= UIBarButtonSystemItemAction 9)\n\
(= UIBarButtonSystemItemOrganize 10)\n\
(= UIBarButtonSystemItemBookmarks 11)\n\
(= UIBarButtonSystemItemSearch 12)\n\
(= UIBarButtonSystemItemRefresh 13)\n\
(= UIBarButtonSystemItemStop 14)\n\
(= UIBarButtonSystemItemCamera 15)\n\
(= UIBarButtonSystemItemTrash 16)\n\
(= UIBarButtonSystemItemPlay 17)\n\
(= UIBarButtonSystemItemPause 18)\n\
(= UIBarButtonSystemItemRewind 19)\n\
(= UIBarButtonSystemItemFastForward 20)\n\
(= UIBarButtonSystemItemUndo 21)\n\
(= UIBarButtonSystemItemRedo 22)\n\
(= UIBarButtonSystemItemPageCurl 23)\n\
\n\
(= UIBarButtonItemStylePlain 0)\n\
(= UIBarButtonItemStyleBordered 1)\n\
(= UIBarButtonItemStyleDone 2)\n\
\n\
(= UIToolbarPositionAny 0)\n\
(= UIToolbarPositionBottom 1)\n\
(= UIToolbarPositionTop 2)\n\
\n\
(= UIBarMetricsDefault 0)\n\
(= UIBarMetricsLandscapePhone 1)\n\
\n\
(= UIBarStyleDefault 0)\n\
(= UIBarStyleBlack 1)\n\
\n\
(= UITextAlignmentLeft 0)\n\
(= UITextAlignmentCenter 1)\n\
(= UITextAlignmentRight 2)\n\
\n\
(= UIActivityIndicatorViewStyleWhiteLarge 0)\n\
(= UIActivityIndicatorViewStyleWhite 1)\n\
(= UIActivityIndicatorViewStyleGray 2)\n\
\n\
(= UITouchPhaseBegan 0)\n\
(= UITouchPhaseMoved 1)\n\
(= UITouchPhaseStationary 2)\n\
(= UITouchPhaseEnded 3)\n\
(= UITouchPhaseCancelled 4)\n\
\n\
(= UIUserInterfaceIdiomPhone 0)\n\
(= UIUserInterfaceIdiomPad 1)\n\
\n\
(def is-iphone ()\n\
     (if (eq ((UIDevice currentDevice) userInterfaceIdiom) UIUserInterfaceIdiomPhone)\n\
\t 1\n\
       (else 0)))\n\
\n\
(= UISegmentedControlStylePlain 0)\n\
(= UISegmentedControlStyleBordered 1)\n\
(= UISegmentedControlStyleBar 2)\n\
(= UISegmentedControlStyleBezeled 3)\n\
\n\
(def hide-view (v)\n\
     (if (v superview)\n\
\t (v removeFromSuperview)))\n\
\n\
(def show-view (v parent-v)\n\
     (unless (v superview)\n\
       (parent-v addSubview:v)))\n\
\n\
(def zero-origin (r) (list 0 0 (rect-w r) (rect-h r)))\n\
\n\
(def reset-image-picker-popover-vc ()\n\
     (= image-picker-popover-vc nil))\n\
\n\
(reset-image-picker-popover-vc)\n\
\n\
(def dismiss-image-picker ()\n\
     (if image-picker-popover-vc\n\
\t (image-picker-popover-vc dismissPopoverAnimated:1)\n\
       (reset-image-picker-popover-vc)\n\
       (else (root-vc dismissModalViewControllerAnimated:1))))\n\
\n\
(def has-photo-library ()\n\
     (UIImagePickerController isSourceTypeAvailable:UIImagePickerControllerSourceTypePhotoLibrary))\n\
\n\
(def present-photo-library-ipad (delegate bbi)\n\
     (if (has-photo-library)\n\
\t (let ((picker (new UIImagePickerController)))\n\
\t   (picker setSourceType:UIImagePickerControllerSourceTypePhotoLibrary)\n\
\t   (picker setMediaTypes:(NSArray arrayWithObject:kUTTypeImage))\n\
\t   (picker setAllowsEditing:0)\n\
\t   (picker setDelegate:delegate)\n\
\t   (= image-picker-is-camera 0)\n\
\t   (= image-picker-popover-vc ((UIPopoverController alloc) initWithContentViewController:picker))\n\
\t   (image-picker-popover-vc setDelegate:delegate)\n\
\t   (image-picker-popover-vc presentPopoverFromBarButtonItem:bbi permittedArrowDirections:UIPopoverArrowDirectionAny animated:1))))\n\
\n\
(def photo-library-popover (delegate bbi)\n\
     (if image-picker-popover-vc\n\
\t (dismiss-image-picker)\n\
       (else (present-photo-library-ipad delegate bbi))))\n\
\n\
(def present-photo-library-iphone (delegate)\n\
     (if (has-photo-library)\n\
\t (let ((picker (new UIImagePickerController)))\n\
\t   (picker setSourceType:UIImagePickerControllerSourceTypePhotoLibrary)\n\
\t   (picker setMediaTypes:(NSArray arrayWithObject:kUTTypeImage))\n\
\t   (picker setAllowsEditing:0)\n\
\t   (picker setDelegate:delegate)\n\
\t   (= image-picker-is-camera 0)\n\
\t   (root-vc presentViewController:picker animated:1 completion:nil))))\n\
\n\
(def present-photo-library (delegate bbi)\n\
     (if (is-iphone)\n\
\t (present-photo-library-iphone delegate)\n\
       (else (photo-library-popover delegate bbi))))\n\
\n\
(def has-camera ()\n\
     (UIImagePickerController isSourceTypeAvailable:UIImagePickerControllerSourceTypeCamera))\n\
     \n\
(def present-camera (v)\n\
     (if (has-camera)\n\
\t (let ((picker (new UIImagePickerController)))\n\
\t   (picker setSourceType:UIImagePickerControllerSourceTypeCamera)\n\
\t   (picker setMediaTypes:(NSArray arrayWithObject:kUTTypeImage))\n\
\t   (picker setAllowsEditing:0)\n\
\t   (picker setDelegate:v)\n\
\t   (= image-picker-is-camera 1)\n\
\t   (root-vc presentViewController:picker animated:1 completion:nil))))\n\
\n\
(def length (lst)\n\
     (if lst\n\
\t (+ 1 (length (cdr lst)))\n\
       (else 0)))\n\
\n\
(def present-action-sheet (title other v bbi delegate)\n\
     (let ((as ((UIActionSheet alloc) init)))\n\
       (as setDelegate:delegate)\n\
       (map (do (a) (as addButtonWithTitle:a)) other)\n\
       (if (is-iphone)\n\
\t   (as addButtonWithTitle:\"Cancel\")\n\
\t (as setCancelButtonIndex:(length other))\n\
\t (as showInView:v)\n\
\t (else (as showFromBarButtonItem:bbi animated:1)))))\n\
       \n\
(= SKPaymentTransactionStatePurchasing 0)\n\
(= SKPaymentTransactionStatePurchased 1)\n\
(= SKPaymentTransactionStateFailed 2)\n\
(= SKPaymentTransactionStateRestored 3)\n\
\n\
(= CPTScatterPlotFieldX 0)\n\
(= CPTScatterPlotFieldY 1)\n\
\n\
(def start-record () (AudioGlue startRecord))\n\
(def stop-record () (AudioGlue stopRecord))\n\
(def record-data () (AudioGlue recordData))\n\
\n\
(= UITableViewStylePlain 0)\n\
(= UITableViewStyleGrouped 1)\n\
\n\
(= UITableViewCellEditingStyleNone 0)\n\
(= UITableViewCellEditingStyleDelete 1)\n\
(= UITableViewCellEditingStyleInsert 2)\n\
\n\
(= UITableViewScrollPositionNone 0)\n\
(= UITableViewScrollPositionTop 1)\n\
(= UITableViewScrollPositionMiddle 2)\n\
(= UITableViewScrollPositionBottom 3)\n\
\n\
(= UITableViewRowAnimationFade 0)\n\
(= UITableViewRowAnimationRight 1)\n\
(= UITableViewRowAnimationLeft 2)\n\
(= UITableViewRowAnimationTop 3)\n\
(= UITableViewRowAnimationBottom 4)\n\
(= UITableViewRowAnimationNone 5)\n\
(= UITableViewRowAnimationMiddle 6)\n\
(= UITableViewRowAnimationAutomatic 100)\n\
\n\
(= UIKeyboardTypeDefault 0)\n\
(= UIKeyboardTypeASCIICapable 1)\n\
(= UIKeyboardTypeNumbersAndPunctuation 2)\n\
(= UIKeyboardTypeURL 3)\n\
(= UIKeyboardTypeNumberPad 4)\n\
(= UIKeyboardTypePhonePad 5)\n\
(= UIKeyboardTypeNamePhonePad 6)\n\
(= UIKeyboardTypeEmailAddress 7)\n\
(= UIKeyboardTypeDecimalPad 8)\n\
(= UIKeyboardTypeTwitter 9)\n\
(= UIKeyboardTypeAlphabet UIKeyboardTypeASCIICapable)\n\
\n\
(= UIReturnKeyDefault 0)\n\
(= UIReturnKeyGo 1)\n\
(= UIReturnKeyGoogle 2)\n\
(= UIReturnKeyJoin 3)\n\
(= UIReturnKeyNext 4)\n\
(= UIReturnKeyRoute 5)\n\
(= UIReturnKeySearch 6)\n\
(= UIReturnKeySend 7)\n\
(= UIReturnKeyYahoo 8)\n\
(= UIReturnKeyDone 9)\n\
(= UIReturnKeyEmergencyCall 10)\n\
\n\
(= UITextFieldViewModeNever 0)\n\
(= UITextFieldViewModeWhileEditing 1)\n\
(= UITextFieldViewModeUnlessEditing 2)\n\
(= UITextFieldViewModeAlways 3)\n\
\n\
(= UIControlContentVerticalAlignmentCenter 0)\n\
(= UIControlContentVerticalAlignmentTop 1)\n\
(= UIControlContentVerticalAlignmentBottom 2)\n\
(= UIControlContentVerticalAlignmentFill 3)\n\
\n\
(= UISwipeGestureRecognizerDirectionRight 1)\n\
(= UISwipeGestureRecognizerDirectionLeft 2)\n\
(= UISwipeGestureRecognizerDirectionUp 4)\n\
(= UISwipeGestureRecognizerDirectionDown 8)\n\
\n\
(bridge constant kCAGravityCenter \"@\")\n\
(bridge constant kCAGravityTop \"@\")\n\
(bridge constant kCAGravityBottom \"@\")\n\
(bridge constant kCAGravityLeft \"@\")\n\
(bridge constant kCAGravityRight \"@\")\n\
(bridge constant kCAGravityTopLeft \"@\")\n\
(bridge constant kCAGravityTopRight \"@\")\n\
(bridge constant kCAGravityBottomLeft \"@\")\n\
(bridge constant kCAGravityBottomRight \"@\")\n\
(bridge constant kCAGravityResize \"@\")\n\
(bridge constant kCAGravityResizeAspect \"@\")\n\
(bridge constant kCAGravityResizeAspectFill \"@\")\n\
\n\
(= kCGBlendModeNormal 0)\n\
(= kCGBlendModeMultiply 1)\n\
(= kCGBlendModeScreen 2)\n\
(= kCGBlendModeOverlay 3)\n\
(= kCGBlendModeDarken 4)\n\
(= kCGBlendModeLighten 5)\n\
(= kCGBlendModeColorDodge 6)\n\
(= kCGBlendModeColorBurn 7)\n\
(= kCGBlendModeSoftLight 8)\n\
(= kCGBlendModeHardLight 9)\n\
(= kCGBlendModeDifference 10)\n\
(= kCGBlendModeExclusion 11)\n\
(= kCGBlendModeHue 12)\n\
(= kCGBlendModeSaturation 13)\n\
(= kCGBlendModeColor 14)\n\
(= kCGBlendModeLuminosity 15)\n\
(= kCGBlendModeClear 16)\n\
(= kCGBlendModeCopy 17)\n\
(= kCGBlendModeSourceIn 18)\n\
(= kCGBlendModeSourceOut 19)\n\
(= kCGBlendModeSourceAtop 20)\n\
(= kCGBlendModeDestinationOver 21)\n\
(= kCGBlendModeDestinationIn 22)\n\
(= kCGBlendModeDestinationOut 23)\n\
(= kCGBlendModeDestinationAtop 24)\n\
(= kCGBlendModeXOR 25)\n\
(= kCGBlendModePlusDarker 26)\n\
(= kCGBlendModePlusLighter 27)\n\
\n\
(class GestureHandler is NSObject\n\
       (- (void)panHandler:(id)gr is\n\
\t  (let ((p (gr translationInView:((gr view) superview)))\n\
\t\t(r ((gr view) frame)))\n\
\t    ((gr view) setFrame:(list (+ (rect-x r) (point-x p))\n\
\t\t\t\t (+ (rect-y r) (point-y p))\n\
\t\t\t\t (rect-w ((gr view) frame))\n\
\t\t\t\t (rect-h ((gr view) frame))))\n\
\t    (gr setTranslation:'(0 0) inView:((gr view) superview))))\n\
       (- (void)doubleTapHandler:(id)gr is\n\
\t  ((gr view) removeFromSuperview))\n\
       (- (void)pinchHandler:(id)gr is\n\
\t  (let ((w (* (rect-w ((gr view) frame)) (gr scale)))\n\
\t\t(h (* (rect-h ((gr view) frame)) (gr scale))))\n\
\t    ((gr view) setBounds:(list 0 0 w h))\n\
\t    (gr setScale:1.0))))\n\
(= movable-handler (new GestureHandler))\n\
\n\
(= UIGestureRecognizerStatePossible 0)\n\
(= UIGestureRecognizerStateBegan 1)\n\
(= UIGestureRecognizerStateChanged 2)\n\
(= UIGestureRecognizerStateEnded 3)\n\
(= UIGestureRecognizerStateCancelled 4)\n\
(= UIGestureRecognizerStateFailed 5)\n\
(= UIGestureRecognizerStateRecognized UIGestureRecognizerStateEnded)\n\
\n\
(def add-pan-gr (v n target action)\n\
     (let ((gr ((UIPanGestureRecognizer alloc) initWithTarget:target action:action)))\n\
       (gr setMinimumNumberOfTouches:n)\n\
       (v addGestureRecognizer:gr)))\n\
\n\
(def add-tap-gr (v n target action)\n\
     (let ((gr ((UITapGestureRecognizer alloc) initWithTarget:target action:action)))\n\
\t(gr setNumberOfTapsRequired:n)\n\
\t(v addGestureRecognizer:gr)))\n\
\n\
(def add-double-tap-gr (v target action) (add-tap-gr v 2 target action))\n\
\n\
(def add-pinch-gr (v target action)\n\
     (let ((gr ((UIPinchGestureRecognizer alloc) initWithTarget:target action:action)))\n\
\t(v addGestureRecognizer:gr)))\n\
\n\
(= UISwipeGestureRecognizerDirectionRight 1)\n\
(= UISwipeGestureRecognizerDirectionLeft 2)\n\
(= UISwipeGestureRecognizerDirectionUp 4)\n\
(= UISwipeGestureRecognizerDirectionDown 8)\n\
\n\
(def add-swipe-gr (v n direction target action)\n\
     (let ((gr ((UISwipeGestureRecognizer alloc) initWithTarget:target action:action)))\n\
       (gr setNumberOfTouchesRequired:n)\n\
       (gr setDirection:direction)\n\
       (v addGestureRecognizer:gr)))\n\
(def add-swipe-left-gr (v target action)\n\
     (add-swipe-gr v 1 UISwipeGestureRecognizerDirectionLeft target action))\n\
(def add-swipe-right-gr (v target action)\n\
     (add-swipe-gr v 1 UISwipeGestureRecognizerDirectionRight target action))\n\
(def add-swipe-up-gr (v target action)\n\
     (add-swipe-gr v 1 UISwipeGestureRecognizerDirectionUp target action))\n\
(def add-swipe-down-gr (v target action)\n\
     (add-swipe-gr v 1 UISwipeGestureRecognizerDirectionDown target action))\n\
\n\
(def white-color () (UIColor whiteColor))\n\
(def clear-color () (UIColor clearColor))\n\
(def black-color () (UIColor blackColor))\n\
(def green-color () (UIColor greenColor))\n\
\n\
(def set-view-border (v w color)\n\
     ((v layer) setBorderWidth:w)\n\
     ((v layer) setBorderColor:(color CGColor)))\n\
\n\
(def set-view-color (v)\n\
     ((v layer) setCornerRadius:10.0)\n\
     ((v layer) setMasksToBounds:1))\n\
\n\
(def ratio (val n d)\n\
     (* (/ val d) n))\n\
\n\
(def delta-rect (r dx dy dw dh)\n\
     (list\n\
      (+ (rect-x r) dx)\n\
      (+ (rect-y r) dy)\n\
      (+ (rect-w r) dw)\n\
      (+ (rect-h r) dh)))\n\
\n\
(bridge constant kEAGLColorFormatRGB565 \"@\")\n\
(bridge constant kEAGLColorFormatRGBA8 \"@\")\n\
\n\
(= UILineBreakModeWordWrap 0)\n\
(= UILineBreakModeCharacterWrap 1)\n\
(= UILineBreakModeClip 2)\n\
(= UILineBreakModeHeadTruncation 3)\n\
(= UILineBreakModeTailTruncation 4)\n\
(= UILineBreakModeMiddleTruncation 5)\n\
\n\
(= UIStatusBarAnimationNone 0)\n\
(= UIStatusBarAnimationFade 1)\n\
(= UIStatusBarAnimationSlide 2)\n\
\n\
(def set-status-bar-hidden (val) (app setStatusBarHidden:(bool val) withAnimation:UIStatusBarAnimationFade))\n\
(def show-status-bar () (set-status-bar-hidden 0))\n\
(def hide-status-bar () (set-status-bar-hidden 1))\n\
\n\
(= UIScrollViewIndicatorStyleDefault 0)\n\
(= UIScrollViewIndicatorStyleBlack 1)\n\
(= UIScrollViewIndicatorStyleWhite 2)\n\
\n\
(= UIViewAnimationOptionLayoutSubviews (<< 1 0))\n\
(= UIViewAnimationOptionAllowUserInteraction (<< 1 1))\n\
(= UIViewAnimationOptionBeginFromCurrentState (<< 1 2))\n\
(= UIViewAnimationOptionRepeat (<< 1 3))\n\
(= UIViewAnimationOptionAutoreverse (<< 1 4))\n\
(= UIViewAnimationOptionOverrideInheritedDuration (<< 1 5))\n\
(= UIViewAnimationOptionOverrideInheritedCurve (<< 1 6))\n\
(= UIViewAnimationOptionAllowAnimatedContent (<< 1 7))\n\
(= UIViewAnimationOptionShowHideTransitionViews (<< 1 8))\n\
\n\
(= UIViewAnimationOptionCurveEaseInOut (<< 0 16))\n\
(= UIViewAnimationOptionCurveEaseIn (<< 1 16))\n\
(= UIViewAnimationOptionCurveEaseOut (<< 2 16))\n\
(= UIViewAnimationOptionCurveLinear (<< 3 16))\n\
\n\
(= UIViewAnimationOptionTransitionNone (<< 0 20))\n\
(= UIViewAnimationOptionTransitionFlipFromLeft (<< 1 20))\n\
(= UIViewAnimationOptionTransitionFlipFromRight (<< 2 20))\n\
(= UIViewAnimationOptionTransitionCurlUp (<< 3 20))\n\
(= UIViewAnimationOptionTransitionCurlDown (<< 4 20))\n\
(= UIViewAnimationOptionTransitionCrossDissolve (<<  5 20))\n\
(= UIViewAnimationOptionTransitionFlipFromTop (<< 6 20))\n\
(= UIViewAnimationOptionTransitionFlipFromBottom (<< 7 20))\n\
\n\
(bridge constant UIApplicationLaunchOptionsURLKey \"@\")\n\
(bridge constant UIApplicationLaunchOptionsSourceApplicationKey \"@\")\n\
(bridge constant UIApplicationLaunchOptionsRemoteNotificationKey \"@\")\n\
(bridge constant UIApplicationLaunchOptionsAnnotationKey \"@\")\n\
(bridge constant UIApplicationLaunchOptionsLocalNotificationKey \"@\")\n\
(bridge constant UIApplicationLaunchOptionsLocationKey \"@\")\n\
(bridge constant UIApplicationLaunchOptionsNewsstandDownloadsKey \"@\")\n\
\n\
(def write-image (image path) (Glue writeImage:image path:path))\n\
\n\
(= UIEventSubtypeMotionShake 1)\n\
\n\
(= UIAlertViewStyleDefault 0)\n\
(= UIAlertViewStyleSecureTextInput 1)\n\
(= UIAlertViewStylePlainTextInput 2)\n\
(= UIAlertViewStyleLoginAndPasswordInput 3)\n\
\n\
(= NSNotFound 2147483647)\n\
\n\
(class ConsoleHandler is NSObject\n\
       (- (void)alertView:(id)av clickedButtonAtIndex:(int)index is\n\
\t  (prn \"alert clicked button at index\"))\n\
       (- (void)alertView:(id)av didDismissWithButtonIndex:(int)index is\n\
\t  (prn \"alert did dismiss with button index\"))\n\
       (- (void)alertView:(id)av willDismissWithButtonIndex:(int)index is\n\
\t  (prn \"alert will dismiss with button index\"))\n\
       (- (void)alertViewCancel:(id)av is\n\
\t  (prn \"alert cancel\"))\n\
       (- (void)didPresentAlertView:(id)av is\n\
\t  (prn \"alert did present\"))\n\
       (- (void)willPresentAlertView:(id)av is\n\
\t  (prn \"alert will present\")\n\
\t  (set @alert-v av)\n\
\t  (set @tf-should-return nil)\n\
\t  (let ((tf (av textFieldAtIndex:0)))\n\
\t    (tf setDelegate:self)))\n\
       (- (int)textFieldShouldReturn:(id)tf is\n\
\t  (set @tf-should-return 1)\n\
\t  1)\n\
       (- (void)textFieldDidEndEditing:(id)tf is\n\
\t  (@alert-v dismissWithClickedButtonIndex:0 animated:0)\n\
\t  (set @alert-v nil)\n\
\t  (set @tf-should-return nil)\n\
\t  (eval (parse \"(#{(tf text)})\"))))\n\
\n\
(= console-handler (new ConsoleHandler))\n\
\n\
(def show-console ()\n\
     (let ((v ((UIAlertView alloc) initWithTitle:\"Console\" message:\"Type something...\" delegate:console-handler cancelButtonTitle:\"Cancel\" otherButtonTitles:nil)))\n\
       (v setAlertViewStyle:UIAlertViewStylePlainTextInput)\n\
       (v show)\n\
       v))\n\
\n\
(class NuWebViewController is UIViewController\n\
       (- (id) initWithPath:(id) path is\n\
\t  (super init)\n\
\t  (add-right-bbi self)\n\
\t  (set @request (NSURLRequest requestWithURL:(NSURL fileURLWithPath:path)))\n\
\t  self)\n\
       (- (void) loadView is\n\
\t  (self setView:(Glue UIWebView:'(0 0 0 0)))\n\
\t  ((self view) setScalesPageToFit:1))\n\
       (- (void) viewDidLoad is\n\
\t  ((self view) loadRequest:@request))\n\
       (- (int)shouldAutorotateToInterfaceOrientation:(int)orientation is 1))\n\
\n\
(def url-w-path (path) (NSURL fileURLWithPath:path))\n\
\n\
(def new-web-v (frame url)\n\
     (let ((v (Glue UIWebView:frame)))\n\
       (v setScalesPageToFit:1)\n\
       (v loadRequest:(NSURLRequest requestWithURL:url))\n\
       v))\n\
\n\
(def load-web-file (name)\n\
     (= web-v (new-web-v (root-f) (url-w-path (path-in-cwd name))))\n\
     (add-subview root-v web-v))\n\
\n\
(class ConnectionHandler is NSObject\n\
       (- (id)init is\n\
\t  (super init)\n\
\t  (set @data (NSMutableData data))\n\
\t  self)\n\
       (- (void)connection:(id)connection didReceiveResponse:(id)response is\n\
\t  (@data setLength:0))\n\
       (- (void)connection:(id)connection didReceiveData:(id)data is\n\
\t  (@data appendData:data))\n\
       (- (void)connection:(id)connection didFailWithError:(id)error is\n\
\t  (prn \"connection failed\"))\n\
       (- (void)connectionDidFinishLoading:(id)connection is\n\
\t  (prn \"connection finished loading\")))\n\
       \n\
(def new-url-connection (addr)\n\
     (let ((req (NSURLRequest requestWithURL:(NSURL URLWithString:addr)))\n\
\t   (h ((ConnectionHandler alloc) init)))\n\
       (let ((conn ((NSURLConnection alloc) initWithRequest:req delegate:h)))\n\
\t (if conn\n\
\t     h\n\
\t   (else nil)))))\n\
\n\
(def arr-elt (arr lst)\n\
     (if lst\n\
\t (arr-elt (nth (car lst) arr) (cdr lst))\n\
       (else arr)))\n\
\n\
(def arr-elt-to-list (arr lst)\n\
     (if lst\n\
\t (cons (arr-elt arr (car lst)) (arr-elt-to-list arr (cdr lst)))\n\
       (else nil)))\n\
\n\
(def nth-in-array (n arr) (arr objectAtIndex:n))\n\
(def nth-arr (n arr) (arr objectAtIndex:n))\n\
(def len-arr (arr) (arr count))\n\
\n\
(def nth (n lst)\n\
     (if (atom n)\n\
\t (lst objectAtIndex:n)\n\
       (else (arr-elt lst n))))\n\
\n\
(def join-strings (separator *strs)\n\
     ((list-to-array *strs) componentsJoinedByString:separator))\n\
\n\
(def add-to-array (arr lst)\n\
     (if lst\n\
\t (arr addObject:(car lst))\n\
       (add-to-array arr (cdr lst))))\n\
\n\
(def list-to-array (lst)\n\
     (let ((arr (new NSMutableArray)))\n\
       (add-to-array arr lst)\n\
       arr))\n\
\n\
(def to-array (*a) (list-to-array *a))\n\
\n\
(def bool (val) (if val 1 (else 0)))\n\
\n\
(def golden-ratio-lg (n) (/ n 1.618))\n\
(def golden-ratio-sm (n) (- n (golden-ratio-lg n)))\n\
\n\
(def text-size (str font) (str sizeWithFont:font))\n\
(def text-width (str font) (size-w (text-size str font)))\n\
(def text-height (str font) (size-h (text-size str font)))\n\
\n\
(def system-font-size () (UIFont systemFontSize))\n\
(def system-font-of-size (size) (UIFont systemFontOfSize:size))\n\
(def system-font () (system-font-of-size (system-font-size)))\n\
(def bold-system-font-of-size (size) (UIFont boldSystemFontOfSize:size))\n\
(def bold-system-font () (bold-system-font-of-size (system-font-size)))\n\
(def bold-system-font-height () (text-height \"X\" (bold-system-font)))\n\
(def small-system-font () (UIFont systemFontOfSize:(UIFont smallSystemFontSize)))\n\
(def small-system-font-s () (\"X\" sizeWithFont:(UIFont systemFontOfSize:(UIFont smallSystemFontSize))))\n\
(def small-system-font-w () (size-w (small-system-font-s)))\n\
(def small-system-font-h () (size-h (small-system-font-s)))\n\
(def small-bold-system-font () (UIFont boldSystemFontOfSize:(UIFont smallSystemFontSize)))\n\
\n\
(def deg2rad (angle) (* angle 0.01745329252))\n\
(def rad2deg (angle) (* angle 57.29577951))\n\
\n\
(mac iter (var start end body)\n\
     `(for ((set ,var ,start) (< ,var ,end) (set ,var (+ ,var 1)))\n\
\t  ,body))\n\
\n\
(= kCCDirectorTypeNSTimer 0)\n\
(= kCCDirectorTypeMainLoop 1)\n\
(= kCCDirectorTypeThreadMainLoop 2)\n\
(= kCCDirectorTypeDisplayLink 3)\n\
\n\
(= kCCDeviceOrientationPortrait UIDeviceOrientationPortrait)\n\
(= kCCDeviceOrientationPortraitUpsideDown UIDeviceOrientationPortraitUpsideDown)\n\
(= kCCDeviceOrientationLandscapeLeft UIDeviceOrientationLandscapeLeft)\n\
(= kCCDeviceOrientationLandscapeRight UIDeviceOrientationLandscapeRight)\n\
\n\
(= kTexture2DPixelFormat_Automatic 0)\n\
(= kTexture2DPixelFormat_RGBA8888 1)\n\
(= kTexture2DPixelFormat_RGB565 2)\n\
\n\
(= UITextAutocapitalizationTypeNone 0)\n\
(= UITextAutocapitalizationTypeWords 1)\n\
(= UITextAutocapitalizationTypeSentences 2)\n\
(= UITextAutocapitalizationTypeAllCharacters 3)\n\
\n\
(= UITextAutocorrectionTypeDefault 0)\n\
(= UITextAutocorrectionTypeNo 1)\n\
(= UITextAutocorrectionTypeYes 2)\n\
\n\
(= UITextSpellCheckingTypeDefault 0)\n\
(= UITextSpellCheckingTypeNo 1)\n\
(= UITextSpellCheckingTypeYes 2)\n\
\n\
(= NSOperationQueuePriorityVeryLow -8)\n\
(= NSOperationQueuePriorityLow -4)\n\
(= NSOperationQueuePriorityNormal 0)\n\
(= NSOperationQueuePriorityHigh 4)\n\
(= NSOperationQueuePriorityVeryHigh 8)\n\
",

	"soledjvu", "\
\n\
(def new-flexible-bbi ()\n\
     ((UIBarButtonItem alloc) initWithBarButtonSystemItem:UIBarButtonSystemItemFlexibleSpace target:nil action:nil))\n\
\n\
(= transparent-image (image-from-file (resolve-bundle \"transparent.png\")))\n\
(= flexible-bbi (new-flexible-bbi))\n\
\n\
\n\
(def new-navigation-bar-v (frame)\n\
     (let ((v ((UINavigationBar alloc) initWithFrame:frame)))\n\
       v))\n\
\n\
(def center-pos (lg sm)\n\
     (/ (- lg sm) 2))\n\
\n\
(def center-if-smaller (a b)\n\
     (if (< b a)\n\
\t (center-pos (a b))\n\
       (else 0)))\n\
\n\
(def center-rect-if-smaller (a b)\n\
     (list (center-if-smaller (rect-x a) (rect-x b))\n\
\t   (center-if-smaller (rect-y a) (rect-y b))\n\
\t   (rect-w b)\n\
\t   (rect-h b)))\n\
\n\
(def center-view-in-superview (v)\n\
     (let ((a ((v superview) bounds))\n\
\t   (b (v frame)))\n\
       (v setFrame:(center-rect-if-smaller a b))))\n\
\n\
(def path-display-name (path) ((path lastPathComponent) stringByDeletingPathExtension))\n\
(def sort-file-array-alphabetically (arr) (Glue sortFileArrayAlphabetically:arr))\n\
\n\
(class ViewController is UIViewController\n\
       (- (id)initWithView:(id)v is\n\
\t  (super init)\n\
\t  (set @v v)\n\
\t  self)\n\
       (- (void)loadView is\n\
\t  (self setView:@v))\n\
       (- (int)shouldAutorotateToInterfaceOrientation:(int)orientation is\n\
\t  (vc-should-rotate orientation))\n\
       (- (void)willAnimateRotationToInterfaceOrientation:(int)orientation duration:(double)duration is\n\
\t  (vc-will-animate-rotation orientation duration))\n\
       (- (void)willRotateToInterfaceOrientation:(int)orientation duration:(double)duration is\n\
\t  (vc-will-rotate orientation duration))\n\
       (- (void)didRotateFromInterfaceOrientation:(int)orientation is\n\
\t  (vc-did-rotate orientation)))\n\
\n\
(def new-vc (v)\n\
     ((ViewController alloc) initWithView:v))\n\
\n\
(def new-nav-vc (rvc)\n\
     (let ((vc ((UINavigationController alloc) initWithRootViewController:rvc)))\n\
       (hide-vc-nav-bar vc)\n\
       vc))\n\
(def set-vc (vc) (window setRootViewController:vc))\n\
(def hide-vc-nav-bar (vc) (vc setNavigationBarHidden:1))\n\
(def show-vc-nav-bar (vc) (vc setNavigationBarHidden:0))\n\
\n\
(def save-str-to-settings (key val) ((NSUserDefaults standardUserDefaults) setObject:val forKey:key))\n\
(def save-int-to-settings (key val) (save-str-to-settings key \"#{val}\"))\n\
(def load-str-from-settings (key def)\n\
     (let ((val ((NSUserDefaults standardUserDefaults) stringForKey:key)))\n\
       (if val val (else def))))\n\
(def load-int-from-settings (key def)\n\
     (let ((val (load-str-from-settings key nil)))\n\
       (if val (val intValue) (else def))))\n\
\n\
(class RootView is UIView\n\
       (- (id)initWithLayout:(id)layout-fn is\n\
\t  (super initWithFrame:(app-frame))\n\
\t  (set-autoresizing-mask self w h)\n\
\t  (set @layout-fn layout-fn)\n\
\t  self)\n\
       (- (void)layoutSubviews is\n\
\t  (if @layout-fn (@layout-fn))))\n\
\n\
(def root-f () (root-v frame))\n\
(def root-x () (rect-x (root-f)))\n\
(def root-y () (rect-y (root-f)))\n\
(def root-w () (rect-w (root-f)))\n\
(def root-h () (rect-h (root-f)))\n\
\n\
(def new-root-view (layout) ((RootView alloc) initWithLayout:layout))\n\
\n\
(def add-subview (super-v sub-v)\n\
     (super-v addSubview:sub-v)\n\
     sub-v)\n\
\n\
(def add-subview-below (super-v sub-v below-v)\n\
     (super-v insertSubview:sub-v belowSubview:below-v))\n\
\n\
(def handle-tap (gr)\n\
     (if (prefs-show-search)\n\
\t (hide-search)\n\
       (else\n\
\t(if (prefs-hide-sidebar)\n\
\t    (if (eq (gr state) UIGestureRecognizerStateEnded) (toggle-controls))\n\
\t  (else (hide-sidebar))))))\n\
\n\
(class Handler is NSObject\n\
       (- (void)handleHome is (show-sidebar))\n\
       (- (void)handleSearch is (show-search))\n\
       (- (void)handleRefreshButton is (refresh-dir))\n\
       (- (void)handleDocumentsButton is (close-contents))\n\
       (- (void)handleContentsButton is (open-contents))\n\
       (- (void)handleValueChanged is (update-title))\n\
       (- (void)handleSnapValue is (handle-slider-snap-value))\n\
       (- (void)handleTap:(id)gr is (handle-tap gr))\n\
       (- (void)searchBarCancelButtonClicked:(id)sb is\n\
\t  (hide-search))\t\t\t\t    \n\
       (- (void)searchBar:(id)sb textDidChange:(id)str is\n\
\t  (handle-search str))\n\
       (- (void)searchBarSearchButtonClicked:(id)sb is\n\
\t  (update-search))\n\
       \n\
       )\n\
(= handler (new Handler))\n\
\n\
(def new-system-bbi (item target action)\n\
     ((UIBarButtonItem alloc) initWithBarButtonSystemItem:item target:target action:action))\n\
(def new-title-bbi (title style target action)\n\
     ((UIBarButtonItem alloc) initWithTitle:title style:style target:target action:action))\n\
(def new-bordered-title-bbi (title target action)\n\
     (new-title-bbi title UIBarButtonItemStyleBordered target action))\n\
(def new-done-title-bbi (title target action)\n\
     (new-title-bbi title UIBarButtonItemStyleDone target action))\n\
(def new-refresh-bbi (target action)\n\
     (new-system-bbi UIBarButtonSystemItemRefresh target action))\n\
(def new-search-bbi ()\n\
     (new-system-bbi UIBarButtonSystemItemSearch handler \"handleSearch\"))\n\
(def new-custom-bbi (v) ((UIBarButtonItem alloc) initWithCustomView:v))\n\
\n\
(= double-arrow-icon (Image doubleArrowIcon))\n\
(= table-of-contents-icon (Image tableOfContentsIcon))\n\
\n\
(def new-home-bbi ()\n\
     ((UIBarButtonItem alloc) initWithImage:double-arrow-icon style:UIBarButtonItemStylePlain target:handler action:\"handleHome\"))\n\
\n\
(= documents-bbi (new-bordered-title-bbi \"Documents\" handler \"handleDocumentsButton\"))\n\
(def new-contents-bbi ()\n\
     ((UIBarButtonItem alloc) initWithImage:table-of-contents-icon style:UIBarButtonItemStylePlain target:handler action:\"handleContentsButton\"))\n\
(= contents-bbi (new-contents-bbi))\n\
\n\
(= refresh-bbi (new-refresh-bbi handler \"handleRefreshButton\"))\n\
\n\
(= dir-only-tb-items (to-array refresh-bbi flexible-bbi))\n\
(= dir-toc-tb-items (to-array refresh-bbi flexible-bbi contents-bbi))\n\
(= contents-tb-items (to-array documents-bbi flexible-bbi))\n\
\n\
(def set-prefs-hide-sidebar (val) (save-int-to-settings \"hidesidebar\" val))\n\
(def set-prefs-hide-controls (val) (save-int-to-settings \"hidecontrols\" val))\n\
(def set-prefs-current-document (val) (save-str-to-settings \"currentdocument\" val))\n\
(def set-prefs-current-page (val) (save-int-to-settings \"currentpage\" val))\n\
(def set-prefs-show-search (val) (save-int-to-settings \"showsearch\" val))\n\
\n\
(def prefs-hide-sidebar () (load-int-from-settings \"hidesidebar\" 1))\n\
(def prefs-hide-controls () (load-int-from-settings \"hidecontrols\" 0))\n\
(def prefs-current-document () (load-str-from-settings \"currentdocument\" nil))\n\
(def prefs-current-page () (load-int-from-settings \"currentpage\" 0))\n\
(def prefs-show-search () (load-int-from-settings \"showsearch\" 0))\n\
\n\
(def save-prefs-current-document ()\n\
     (if scroll-v\n\
\t (set-prefs-current-document ((scroll-v document) path))\n\
\t (set-prefs-current-page (scroll-v visiblePageIndex))))\n\
\n\
(def load-prefs-current-document ()\n\
     (let ((val (prefs-current-document)))\n\
       (if (and val (file-exists val))\n\
\t   val\n\
\t (else (path-to-default-document)))))\n\
\n\
(def path-to-default-document () (path-in-bundle \"Sample goodytwoshoes.djvu\"))\n\
\n\
(def path-from-ip (ip arr)\n\
     (if (eq (ip section) 0)\n\
\t (path-in-docs (arr objectAtIndex:(ip row)))\n\
       (else\n\
\t(case (ip row)\n\
\t      (0 (path-in-bundle \"License lizard2002.djvu\"))\n\
\t      (1 (path-in-bundle \"License lizard2007.djvu\"))\n\
\t      (else (path-to-default-document))))))\n\
\n\
\n\
\n\
\n\
\n\
(def table-view-cell (tv style id)\n\
     (or (tv dequeueReusableCellWithIdentifier:id)\n\
\t ((UITableViewCell alloc) initWithStyle:style reuseIdentifier:id)))\n\
\n\
(def table-view-cell-default (tv id)\n\
     (table-view-cell tv UITableViewCellStyleDefault id))\n\
\n\
(def table-view-cell-value1 (tv id)\n\
     (table-view-cell tv UITableViewCellStyleValue1 id))\n\
(def table-view-cell-value2 (tv id)\n\
     (table-view-cell tv UITableViewCellStyleValue2 id))\n\
(def table-view-cell-subtitle (tv id)\n\
     (table-view-cell tv UITableViewCellStyleSubtitle id))\n\
\n\
(def init-cell-label (l text)\n\
     (l setText:text)\n\
     (l setTextColor:(UIColor colorWithWhite:0.98 alpha:1.0))\n\
     (l setShadowColor:(UIColor blackColor))\n\
     (l setShadowOffset:'(1 1))\n\
     (l setFont:(bold-system-font))\n\
     (l setLineBreakMode:UILineBreakModeMiddleTruncation)\n\
     (l setNumberOfLines:0))\n\
     \n\
(def no-documents-cell (tv)\n\
     (let ((cell (table-view-cell-default tv \"no-documents-cell\")))\n\
       (cell setUserInteractionEnabled:0)\n\
       (init-cell-label (cell textLabel) \"Use iTunes file sharing, open from Safari, Mail, or other app\")\n\
       cell))\n\
\n\
(def document-cell (tv text)\n\
     (let ((cell (table-view-cell-default tv \"document-cell\")))\n\
       (init-cell-label (cell textLabel) text)\n\
       cell))\n\
\n\
(def contents-cell (tv text)\n\
     (let ((cell (table-view-cell-default tv \"contents-cell\")))\n\
       (init-cell-label (cell textLabel) text)\n\
       ((cell textLabel) setLineBreakMode:UILineBreakModeHeadTruncation)\n\
       cell))\n\
\n\
(class DirTableData is NSObject\n\
       (- (int)numberOfSectionsInTableView:(id)tv is 2)\n\
       (- (int)tableView:(id)tv numberOfRowsInSection:(int)section is\n\
\t  (if (eq section 0)\n\
\t      (max (dir-arr count) 1)\n\
\t    (else 3)))\n\
       (- (id)tableView:(id)tv cellForRowAtIndexPath:(id)ip is\n\
\t  (if (and (eq (ip section) 0) (eq (dir-arr count) 0))\n\
\t      (no-documents-cell tv)\n\
\t    (else (document-cell tv (path-display-name (path-from-ip ip dir-arr))))))\n\
       (- (void)tableView:(id)tv didSelectRowAtIndexPath:(id)ip is\n\
\t  (let ((name (path-from-ip ip dir-arr)))\n\
\t    (open-document name 0)\n\
\t    (tv deselectRowAtIndexPath:ip animated:1)))\n\
       (- (void)tableView:(id)tv commitEditingStyle:(int)style forRowAtIndexPath:(id)ip is\n\
\t  (if (and (eq (ip section) 0) (dir-arr count))\n\
\t      (rm (dir-arr objectAtIndex:(ip row)))\n\
\t    (dir-arr removeObjectAtIndex:(ip row))\n\
\t    (if (dir-arr count)\n\
\t\t(tv deleteRowsAtIndexPaths:(NSArray arrayWithObject:ip) withRowAnimation:UITableViewRowAnimationLeft)\n\
\t      (else (tv reloadData)))))\n\
       (- (int)tableView:(id)tv canEditRowAtIndexPath:(id)ip is\n\
\t      (bool (not (ip section))))\n\
       (- (int)tableView:(id)tv editingStyleForRowAtIndexPath:(id)ip is\n\
\t  (if (and (not (ip section)) (dir-arr count))\n\
\t      UITableViewCellEditingStyleDelete\n\
\t    (else UITableViewCellEditingStyleNone)))\n\
       (- (id)tableView:(id)tv titleForHeaderInSection:(int)section is\n\
\t  (case section\n\
\t\t(0 \"Documents\")\n\
\t\t(1 \"SoleDjVu\")\n\
\t\t(else nil))))\n\
(= dir-table-data (new DirTableData))\n\
\n\
(def new-dir-v ()\n\
     (let ((v ((UITableView alloc) initWithFrame:(dir-f) style:UITableViewStylePlain)))\n\
       (v setBackgroundColor:(UIColor colorWithWhite:0.33 alpha:1.0))\n\
       (v setIndicatorStyle:UIScrollViewIndicatorStyleWhite)\n\
       (v setDataSource:dir-table-data)\n\
       (v setDelegate:dir-table-data)\n\
       v))\n\
\n\
(class ContentsTableData is NSObject\n\
       (- (int)numberOfSectionsInTableView:(id)tv is 1)\n\
       (- (int)tableView:(id)tv numberOfRowsInSection:(int)section is\n\
\t      (doc bookmarkCount))\n\
       (- (id)tableView:(id)tv cellForRowAtIndexPath:(id)ip is\n\
\t  (contents-cell tv (doc bookmarkTitle:(ip row))))\n\
       (- (void)tableView:(id)tv didSelectRowAtIndexPath:(id)ip is\n\
\t  (set-page-index (doc bookmarkPageNum:(ip row)))\n\
\t  (tv deselectRowAtIndexPath:ip animated:1)\n\
\t  (hide-sidebar))\n\
       (- (id)tableView:(id)tv titleForHeaderInSection:(int)section is\n\
\t  (if (eq section 0) \"Table of Contents\" (else nil))))\n\
(= contents-table-data (new ContentsTableData))\n\
\n\
(def new-contents-v ()\n\
     (let ((v ((UITableView alloc) initWithFrame:(contents-f) style:UITableViewStylePlain)))\n\
       (v setBackgroundColor:(UIColor colorWithWhite:0.25 alpha:1.0))\n\
       (v setIndicatorStyle:UIScrollViewIndicatorStyleWhite)\n\
       (v setDataSource:contents-table-data)\n\
       (v setDelegate:contents-table-data)\n\
       v))\n\
\n\
(def new-doc-v ()\n\
     (let ((v ((UIView alloc) initWithFrame:(doc-f))))\n\
       (v setBackgroundColor:(UIColor scrollViewTexturedBackgroundColor))\n\
       v))\n\
\n\
(def new-scroll-v (page)\n\
     (let ((v ((PagingScrollView alloc) initWithFrame:(zero-origin (doc-f)) padding:10.0 document:doc page:page)))\n\
       (v setNuDelegate:update-controls)\n\
       (add-tap-gr v 1 handler \"handleTap:\")\n\
       v))\n\
\n\
(def new-toolbar-v ()\n\
     (let ((v (new-w-frame UIToolbar (toolbar-f))))\n\
       (v setBarStyle:UIBarStyleBlack)\n\
       (v setTranslucent:1)\n\
       v))\n\
\n\
(def new-control-tb ()\n\
     (let ((tb ((UIToolbar alloc) initWithFrame:(control-tb-f))))\n\
       (tb setAlpha:(if (prefs-hide-controls) 0.0 (else 1.0)))\n\
       (tb setBarStyle:UIBarStyleBlack)\n\
       (tb setTranslucent:1)\n\
       (tb setItems:(to-array (new-home-bbi) flexible-bbi (new-custom-bbi title-slider-v) flexible-bbi (new-search-bbi)))\n\
       tb))\n\
\n\
(def new-slider-v ()\n\
     (let ((v ((UISlider alloc) initWithFrame:(slider-f))))\n\
       (v setAlpha:1.0)\n\
       (v setMinimumValue:0.0)\n\
       (v setMaximumValue:1.0)\n\
       (v addTarget:handler action:\"handleValueChanged\" forControlEvents:UIControlEventValueChanged)\n\
       (v addTarget:handler action:\"handleSnapValue\" forControlEvents:UIControlEventTouchUpInside)\n\
       (v addTarget:handler action:\"handleSnapValue\" forControlEvents:UIControlEventTouchUpOutside)\n\
       (v addTarget:handler action:\"handleSnapValue\" forControlEvents:UIControlEventTouchCancel)\n\
       v))\n\
\n\
\n\
(def new-title-v ()\n\
     (let ((v ((UILabel alloc) initWithFrame:(title-f))))\n\
       (v setAlpha:1.0)\n\
       (v setFont:(small-bold-system-font))\n\
       (v setText:(get-slider-text))\n\
       (v setTextColor:(UIColor whiteColor))\n\
       (v setBackgroundColor:(UIColor clearColor))\n\
       (v setTextAlignment:UITextAlignmentCenter)\n\
       (v setLineBreakMode:UILineBreakModeMiddleTruncation)\n\
       (v setAdjustsFontSizeToFitWidth:1)\n\
       v))\n\
\n\
(def new-title-slider-v ()\n\
     (let ((v ((UIView alloc) initWithFrame:(title-slider-f))))\n\
       (add-subview v title-v)\n\
       (add-subview v slider-v)\n\
       v))\n\
\n\
(def doc-f () (list (doc-x) (doc-y) (doc-w) (doc-h)))\n\
(def doc-x () (+ (sidebar-x) (sidebar-w)))\n\
(def doc-y () 0)\n\
(def doc-w () (root-w))\n\
(def doc-h () (root-h))\n\
\n\
(def sidebar-f () (list (sidebar-x) (sidebar-y) (sidebar-w) (sidebar-h)))\n\
(def sidebar-x () (if (prefs-hide-sidebar) (- 0 (sidebar-w)) (else 0)))\n\
(def sidebar-y () 0)\n\
(def sidebar-w () (if (is-iphone) 240.0 (else 320.0)))\n\
(def sidebar-h () (root-h))\n\
\n\
(def dir-f () (list (dir-x) (dir-y) (dir-w) (dir-h)))\n\
(def dir-x () 0.0)\n\
(def dir-y () 0.0)\n\
(def dir-w () (sidebar-w))\n\
(def dir-h () (- (sidebar-h) (toolbar-h)))\n\
\n\
(def contents-f () (list (contents-x) (contents-y) (contents-w) (contents-h)))\n\
(def contents-x () 0.0)\n\
(def contents-y () 0.0)\n\
(def contents-w () (sidebar-w))\n\
(def contents-h () (dir-h))\n\
\n\
(def toolbar-f () (list (toolbar-x) (toolbar-y) (toolbar-w) (toolbar-h)))\n\
(def toolbar-x () 0.0)\n\
(def toolbar-y () (- (sidebar-h) 44.0))\n\
(def toolbar-w () (sidebar-w))\n\
(def toolbar-h () 44.0)\n\
\n\
(def scroll-f () (zero-origin (doc-f)))\n\
(def scroll-x () (rect-x (scroll-f)))\n\
(def scroll-y () (rect-y (scroll-f)))\n\
(def scroll-w () (rect-w (scroll-f)))\n\
(def scroll-h () (rect-h (scroll-f)))\n\
\n\
(def control-tb-f () (list (control-tb-x) (control-tb-y) (control-tb-w) (control-tb-h)))\n\
(def control-tb-x () 0)\n\
(def control-tb-y () (- (doc-h) (control-tb-h)))\n\
(def control-tb-w () (doc-w))\n\
(def control-tb-h () 44.0)\n\
\n\
(def title-f ()\t(list (title-x) (title-y) (title-w) (title-h)))\n\
(def title-x () 0)\n\
(def title-y () 5.0)\n\
(def title-w () (title-slider-w))\n\
(def title-h () (- (control-tb-h) (slider-h) 10.0))\n\
\n\
(def slider-f () (list (slider-x) (slider-y) (slider-w) (slider-h)))\n\
(def slider-x () 0)\n\
(def slider-y () (+ (title-y) (title-h)))\n\
(def slider-w () (title-slider-w))\n\
(def slider-h () 20.0)\n\
\n\
(def title-slider-f () (list (title-slider-x) (title-slider-y) (title-slider-w) (title-slider-h)))\n\
(def title-slider-x () 0)\n\
(def title-slider-y () 0)\n\
(def title-slider-w () (- (control-tb-w) 12.0 20.0 10.0 10.0 20.0 12.0))\n\
(def title-slider-h () (control-tb-h))\n\
\n\
(def animate-sidebar (val)\n\
     (if (!= val (prefs-hide-sidebar))\n\
\t (set-prefs-hide-sidebar (bool val))\n\
       (Glue animateWithDuration:0.3 block:(fn () \n\
\t\t\t\t\t       (root-v setNeedsLayout)\n\
\t\t\t\t\t       (root-v layoutIfNeeded)\n\
\t\t\t\t\t       (control-tb setNeedsLayout)))))\n\
(def hide-sidebar () (animate-sidebar 1) (animate-search 0) (animate-controls 0))\n\
(def show-sidebar () (animate-sidebar 0) (animate-search 0) (animate-controls 1))\n\
(def toggle-sidebar () (if (prefs-hide-sidebar) (show-sidebar) (else (hide-sidebar))))\n\
\n\
(def animate-controls (val)\n\
     (if (!= val (prefs-hide-controls))\n\
\t (set-prefs-hide-controls (bool val))\n\
       (set-status-bar-hidden (bool val))\n\
       (let ((alpha (if val 0.0 (else 1.0))))\n\
\t (Glue animateWithDuration:0.3 block:\n\
\t       (fn () (control-tb setAlpha:alpha))))))\n\
(def hide-controls () (animate-controls 1) (animate-search 0) (animate-sidebar 1))\n\
(def show-controls () (animate-controls 0) (animate-search 0) (animate-sidebar 1))\n\
(def toggle-controls () (if (prefs-hide-controls) (show-controls) (else (hide-controls))))\n\
\n\
(def animate-search (val)\n\
     (if (!= val (prefs-show-search))\n\
\t (set-prefs-show-search (bool val))\n\
       (Glue animateWithDuration:0.3 block:(fn ()\n\
\t\t\t\t\t       (root-v setNeedsLayout)\n\
\t\t\t\t\t       (root-v layoutIfNeeded)\n\
\t\t\t\t\t       (control-tb setNeedsLayout)))\n\
       (if val\n\
\t   (search-bar-v becomeFirstResponder)\n\
\t (else (search-bar-v resignFirstResponder)))))\n\
(def hide-search () (animate-search 0) (animate-sidebar 1) (animate-controls 0))\n\
(def show-search () (animate-search 1) (animate-sidebar 1) (animate-controls 1))\n\
     \n\
(def get-slider-page ()\n\
     (scroll-v pageIndexFromFloat:(slider-v value)))\n\
\n\
(def get-slider-text ()\n\
     \"#{((scroll-v document) title)}: Page #{(+ 1 (get-slider-page))} of #{((scroll-v document) pageCount)}\")\n\
\n\
(def update-title ()\n\
     (title-v setText:(get-slider-text)))\n\
\n\
(def update-slider ()\n\
     (slider-v setValue:(scroll-v floatOfVisiblePage)))\n\
\n\
(def update-controls ()\n\
     (update-slider)\n\
     (update-title))\n\
     \n\
(def set-page-index (index)\n\
     (scroll-v goToPage:index)\n\
     (update-controls))\n\
\n\
(def handle-slider-snap-value ()\n\
     (unless (eq (get-slider-page) (scroll-v visiblePageIndex))\n\
       (set-page-index (get-slider-page))))\n\
\n\
(def set-view-shadow (v)\n\
     ((v layer) setShadowColor:((UIColor blackColor) CGColor))\n\
     ((v layer) setShadowOpacity:1.0)\n\
     ((v layer) setShadowOffset:'(0 0))\n\
     ((v layer) setShadowRadius:15.0)\n\
     ((v layer) setMasksToBounds:0)\n\
     ((v layer) setShadowPath:((UIBezierPath bezierPathWithRect:(v bounds)) CGPath)))\n\
     \n\
(def vc-should-rotate (orientation) 1)\n\
(def vc-will-rotate (orientation duration) nil)\n\
(def vc-will-animate-rotation (orientation duration) nil)\n\
(def vc-did-rotate (orientation) nil)\n\
\n\
(def all-files-sorted () (sort-file-array-alphabetically (all-files-in-docs)))\n\
(def refresh-dir ()\n\
     (= dir-arr (all-files-sorted))\n\
     (dir-v reloadData))\n\
\n\
(def new-doc (name) ((DjvuDocument alloc) initWithPath:name))\n\
\n\
(def close-contents ()\n\
     (if contents-v\n\
\t (refresh-dir)\n\
\t (UIView transitionFromView:contents-v toView:dir-v duration:0.5 options:UIViewAnimationOptionTransitionCurlDown completion:nil)\n\
       (toolbar-v setItems:(dir-toc-tb-items))\n\
       (= contents-v nil)))\n\
\n\
(def open-contents ()\n\
     (close-contents)\n\
     (if (doc bookmarkCount)\n\
\t (= contents-v (new-contents-v))\n\
       (UIView transitionFromView:dir-v toView:contents-v duration:0.5 options:UIViewAnimationOptionTransitionCurlUp completion:nil)\n\
       (toolbar-v setItems:(contents-tb-items))\n\
       (show-sidebar)\n\
       (else (progn\n\
\t       (toolbar-v setItems:(dir-only-tb-items) animated:1)\n\
\t       (hide-sidebar)))))\n\
\n\
(def close-document ()\n\
     (close-contents)\n\
     (toolbar-v setItems:(dir-only-tb-items))\n\
     (scroll-v removeFromSuperview)\n\
     (= scroll-v nil)\n\
     (slider-v removeFromSuperview)\n\
     (= slider-v nil)\n\
     (title-v removeFromSuperview)\n\
     (= title-v nil)\n\
     (control-tb removeFromSuperview)\n\
     (= control-tb nil)\n\
     (= doc nil))\n\
\n\
(def open-document (path page)\n\
     (close-document)\n\
     (= doc (new-doc path))\n\
     (if doc\n\
       (= scroll-v (new-scroll-v page))\n\
       (add-subview doc-v scroll-v)\n\
       (= slider-v (new-slider-v))\n\
       (= title-v (new-title-v))\n\
       (= title-slider-v (new-title-slider-v))\n\
       (= control-tb (new-control-tb))\n\
       (add-subview doc-v control-tb)\n\
       (if (< page (scroll-v pageCount))\n\
\t   (set-page-index page)\n\
\t (else (set-page-index 0)))\n\
       (open-contents)\n\
       (search-bar-v setText:nil)))\n\
\n\
(def launch-document ()\n\
     (let ((url (app-options valueForKey:UIApplicationLaunchOptionsURLKey)))\n\
       (if (url isFileURL)\n\
\t   (prn \"opening launch-document\")\n\
\t   (open-document (url path) 0)\n\
\t (else (open-document (load-prefs-current-document) (prefs-current-page))))))\n\
\n\
(def application-open-url (url source annotation)\n\
     (prn \"application-open-url\")\n\
     (if (url isFileURL)\n\
\t (refresh-dir)\n\
       (open-document (url path) 0)\n\
       1\n\
       (else 0)))\n\
     \n\
(def layout-views ()\n\
     (sidebar-v setFrame:(sidebar-f))\n\
     (dir-v setFrame:(dir-f))\n\
     (contents-v setFrame:(contents-f))\n\
     (toolbar-v setFrame:(toolbar-f))\n\
\n\
     (search-bar-v setFrame:(search-bar-f))\n\
     (search-v setFrame:(search-f))\n\
     \n\
     (doc-v setFrame:(doc-f))\n\
     (scroll-v setFrame:(scroll-f))\n\
     (control-tb setFrame:(control-tb-f))\n\
     (title-slider-v setFrame:(title-slider-f))\n\
     (title-v setFrame:(title-f))\n\
     (slider-v setFrame:(slider-f))\n\
     (update-controls))\n\
\n\
(def soledjvu ()\n\
     (= root-v (new-root-view layout-views))\n\
     (= root-vc (new-vc root-v))\n\
     (root-vc setWantsFullScreenLayout:1)\n\
     (set-status-bar-hidden (if (prefs-hide-controls) 1 (else 0)))\n\
     (= nav-vc (new-nav-vc root-vc))\n\
     (hide-vc-nav-bar nav-vc)\n\
     (set-vc nav-vc)\n\
     (= dir-arr (all-files-sorted))\n\
     (= dir-v (new-dir-v))\n\
     (= contents-v nil)\n\
     (= sidebar-v (new-w-frame UIView (sidebar-f)))\n\
     (set-view-shadow sidebar-v)\n\
     (= toolbar-v (new-toolbar-v))\n\
     (toolbar-v setItems:dir-only-tb-items)\n\
\n\
     (= search-bar-v (new-search-bar-v))\n\
     (= search-table-v (new-search-table-v))\n\
     (= search-v (new-search-v))\n\
     (= search-vc (new SearchViewController))\n\
     (= search-dc (new-search-dc))\n\
     (add-subview root-v search-v)\n\
     \n\
     (= doc-v (new-doc-v))\n\
     (= scroll-v nil)\n\
     (= control-tb nil)\n\
     (= slider-v nil)\n\
     (= title-v nil)\n\
     (add-subview-below root-v doc-v search-v)\n\
     (add-subview root-v sidebar-v)\n\
     (add-subview sidebar-v dir-v)\n\
     (add-subview sidebar-v toolbar-v)\n\
     (launch-document))\n\
\n\
(def application-did-finish-launching ()\n\
     (prn \"application-did-finish-launching\")\n\
     (soledjvu))\n\
\n\
(def application-will-resign-active ()\n\
     (prn \"application-will-resign-active\"))\n\
\n\
(def application-did-enter-background ()\n\
     (prn \"application-did-enter-background\")\n\
     (save-prefs-current-document))\n\
\n\
(def application-will-enter-foreground ()\n\
     (prn \"application-will-enter-foreground\"))\n\
\n\
(def application-did-become-active ()\n\
     (prn \"application-did-become-active\"))\n\
\n\
(def application-will-terminate ()\n\
     (prn \"application-will-terminate\")\n\
     (save-prefs-current-document))\n\
\n\
\n\
(def search-bar-f () (list (search-bar-x) (search-bar-y) (search-bar-w) (search-bar-h)))\n\
(def search-bar-x () 0)\n\
(def search-bar-y () 0)\n\
(def search-bar-w () (search-w))\n\
(def search-bar-h () 44)\n\
\n\
(def search-table-f () (list (search-table-x) (search-table-y) (search-table-w) (search-table-h)))\n\
(def search-table-x () 0)\n\
(def search-table-y () (search-bar-h))\n\
(def search-table-w () (search-w))\n\
(def search-table-h () (- (search-h) (search-bar-h)))\n\
\n\
(def search-f () (list (search-x) (search-y) (search-w) (search-h)))\n\
(def search-x () 0)\n\
(def search-y () (if (prefs-show-search) 0 (else (+ (doc-h) 44))))\n\
(def search-w () (sidebar-w))\n\
(def search-h () (doc-h))\n\
\n\
\n\
\n\
\n\
\n\
\n\
\n\
\n\
\t  \n\
(def search-cell (tv idx)\n\
     (let ((cell (table-view-cell-value1 tv \"search-cell\"))\n\
\t   (page (nth 0 (nth idx (doc searchResults))))\n\
\t   (lines ((nth 1 (nth idx (doc searchResults))) count)))\n\
       ((cell textLabel) setText:\"Page #{page}\")\n\
       ((cell detailTextLabel) setText:\n\
\t(if (eq lines 1)\n\
\t    \"1 matching line\"\n\
\t  (else \"#{lines} matching lines\")))\n\
       cell))\n\
(def search-empty-cell (tv)\n\
     (let ((cell (table-view-cell-value1 tv \"search-empty-cell\")))\n\
       (cell setUserInteractionEnabled:0)\n\
       cell))\n\
\n\
(def search-matching-page-text1 ()\n\
     (if (doc searchResults)\n\
\t (let ((n ((doc searchResults) count)))\n\
\t   (if (eq n 1)\n\
\t       \"1 matching page\"\n\
\t     (else (\"#{((doc searchResults) count)} matching pages\"))))\n\
       (else \"\")))\n\
(def search-matching-page-text ()\n\
     (let ((str (search-matching-page-text1)))\n\
       (if (doc searchTask)\n\
\t   \"Searching...#{str}\"\n\
\t (else str))))\n\
\n\
(class SearchTableData is NSObject\n\
       (- (int)numberOfSectionsInTableView:(id)tv is 1)\n\
       (- (int)tableView:(id)tv numberOfRowsInSection:(int)section is\n\
\t  (max 1 ((doc searchResults) count)))\n\
       (- (id)tableView:(id)tv cellForRowAtIndexPath:(id)ip is\n\
\t  (if ((doc searchResults) count)\n\
\t      (search-cell tv (ip row))\n\
\t    (else (search-empty-cell tv))))\n\
       (- (void)tableView:(id)tv didSelectRowAtIndexPath:(id)ip is\n\
\t  (set-page-index (- ((nth 0 (nth (ip row) (doc searchResults))) intValue) 1))\n\
\t  (tv deselectRowAtIndexPath:ip animated:1)\n\
\t  (hide-search))\n\
       (- (id)tableView:(id)tv titleForHeaderInSection:(int)section is\n\
\t  (if ((doc searchResults) count)\n\
\t      nil\n\
\t    (else (search-matching-page-text))))\n\
       (- (id)tableView:(id)tv titleForFooterInSection:(int)section is\n\
\t  (if ((doc searchResults) count)\n\
\t      (search-matching-page-text)\n\
\t    (else nil))))\n\
(= search-table-data (new SearchTableData))\n\
\n\
(def new-search-bar-v ()\n\
     (let ((sb ((UISearchBar alloc) initWithFrame:(search-bar-f))))\n\
       (sb setPlaceholder:\"Search\")\n\
       (sb setAutocapitalizationType:UITextAutocapitalizationTypeNone)\n\
       (sb setAutocorrectionType:UITextAutocorrectionTypeNo)\n\
       (sb setSpellCheckingType:UITextSpellCheckingTypeNo)\n\
       (sb setTintColor:(UIColor clearColor))\n\
       (sb setDelegate:handler)\n\
       sb))\n\
\n\
(def new-search-table-v ()\n\
     (let ((v ((UIView alloc) initWithFrame:(search-table-f))))\n\
       v))\n\
\n\
(def new-search-v ()\n\
     (let ((v ((UIView alloc) initWithFrame:(search-f))))\n\
       ((v layer) setBorderWidth:1)\n\
       ((v layer) setBorderColor:((UIColor colorWithRed:0.33 green:0.33 blue:0.33 alpha:1.0) CGColor))\n\
       (v setBackgroundColor:(UIColor colorWithRed:0.33 green:0.33 blue:0.33 alpha:1.0))\n\
       (set-view-shadow v)\n\
       (add-subview v search-bar-v)\n\
       v))\n\
\n\
(class SearchViewController is UIViewController\n\
       (- (void)loadView is\n\
\t  (self setView:search-v)))\n\
\n\
\n\
(def new-search-dc ()\n\
     (let ((dc ((UISearchDisplayController alloc) initWithSearchBar:search-bar-v contentsController:search-vc)))\n\
       (dc setDelegate:handler)\n\
       (dc setSearchResultsDataSource:search-table-data)\n\
       (dc setSearchResultsDelegate:search-table-data)\n\
       dc))\n\
\n\
(def update-search ()\n\
     ((search-dc searchResultsTableView) reloadData)\n\
     (scroll-v highlightSearchResults))\n\
\n\
(def handle-search (str)\n\
     (if doc\n\
\t (doc search:str onUpdate:update-search)\n\
       (else (update-search))))\n\
\n\
(if debug-mode\n\
    (if app (application-did-finish-launching)))\n\
\n\
",

	0, 0
};
