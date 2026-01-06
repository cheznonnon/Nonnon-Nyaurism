// Project Nonnon
// copyright (c) nonnon all rights reserved
// License : GPL http://www.gnu.org/copyleft/gpl.html


// [!] : this file is made by DeepSeek AI




/*
// Usage

// Load
NSURL *gifURL = [NSURL fileURLWithPath:@"/path/to/your/animation.gif"];
GIFAnimationView *gifView = [[GIFAnimationView alloc] init];
[gifView loadGIFWithURL:gifURL];

// Start
[gifView startAnimating];

// Pause
[gifView pauseAnimating];

// Resume
[gifView resumeAnimating];

// Stop
[gifView stopAnimating];
*/




#ifndef _H_NONNON_MAC_GIFANIMATIONVIEW
#define _H_NONNON_MAC_GIFANIMATIONVIEW




#import <Cocoa/Cocoa.h>
#import <ImageIO/ImageIO.h>




@interface GIFAnimationView : NSImageView

@property (nonatomic, assign) BOOL isAnimating;
@property (nonatomic, strong) NSArray<NSImage *> *frames;
@property (nonatomic, strong) NSArray<NSNumber *> *frameDelays;
@property (nonatomic, assign) NSInteger currentFrameIndex;
@property (nonatomic, strong) NSTimer *animationTimer;

- (void)loadGIFWithURL:(NSURL *)gifURL;
- (void)startAnimating;
- (void)pauseAnimating;
- (void)resumeAnimating;
- (void)stopAnimating;

@end




@implementation GIFAnimationView

- (void)loadGIFWithURL:(NSURL *)gifURL {
    // 既存のアニメーションを停止
    [self stopAnimating];
    
    // GIF ファイルからフレームを読み込む
    CGImageSourceRef source = CGImageSourceCreateWithURL((__bridge CFURLRef)gifURL, NULL);
    if (!source) {
        NSLog(@"Failed to create image source");
        return;
    }

    // 画像のプロパティ辞書を取得
    CFDictionaryRef properties = CGImageSourceCopyPropertiesAtIndex(source, 0, NULL);
    if (properties) {
        // 幅と高さを取得
        CGFloat width = [(__bridge NSNumber *)CFDictionaryGetValue(properties, kCGImagePropertyPixelWidth) floatValue];
        CGFloat height = [(__bridge NSNumber *)CFDictionaryGetValue(properties, kCGImagePropertyPixelHeight) floatValue];
        
//NSLog(@"Image size: %.0f x %.0f", width, height);
        [self setFrame:NSMakeRect( 0, 0, width, height )];
        
        CFRelease(properties);
    }
 
    size_t frameCount = CGImageSourceGetCount(source);
    NSMutableArray *frames = [NSMutableArray arrayWithCapacity:frameCount];
    NSMutableArray *delays = [NSMutableArray arrayWithCapacity:frameCount];
    
    for (size_t i = 0; i < frameCount; i++) {
        CGImageRef imageRef = CGImageSourceCreateImageAtIndex(source, i, NULL);
        if (imageRef) {
            NSImage *image = [[NSImage alloc] initWithCGImage:imageRef size:NSZeroSize];
            [frames addObject:image];
            CGImageRelease(imageRef);
            
            // フレーム遅延時間を取得
            float delay = [self frameDelayAtIndex:i source:source];
            [delays addObject:@(delay)];
        }
    }
    
    CFRelease(source);
    
    self.frames = frames;
    self.frameDelays = delays;
    self.currentFrameIndex = 0;
    
    if (frames.count > 0) {
        self.image = frames[0];
    }
}

- (float)frameDelayAtIndex:(size_t)index source:(CGImageSourceRef)source {
    float delay = 0.1f; // デフォルト値
    
    CFDictionaryRef properties = CGImageSourceCopyPropertiesAtIndex(source, index, NULL);
    if (properties) {
        CFDictionaryRef gifProperties = CFDictionaryGetValue(properties, kCGImagePropertyGIFDictionary);
        if (gifProperties) {
            NSNumber *number = (__bridge id)CFDictionaryGetValue(gifProperties, kCGImagePropertyGIFUnclampedDelayTime);
            if (number == NULL || [number floatValue] == 0) {
                number = (__bridge id)CFDictionaryGetValue(gifProperties, kCGImagePropertyGIFDelayTime);
            }
            if ([number floatValue] > 0) {
                delay = [number floatValue];
            }
        }
        CFRelease(properties);
    }
    
    // 遅延時間が短すぎる場合の調整
    if (delay < 0.02f) delay = 0.1f;
    
    return delay;
}

- (void)startAnimating {
    if (self.isAnimating || self.frames.count == 0) return;
    
    self.isAnimating = YES;
    self.currentFrameIndex = 0;
    [self animateNextFrame];
}

- (void)animateNextFrame {
    if (!self.isAnimating || self.frames.count == 0) return;
    
    // 現在のフレームを表示
    self.image = self.frames[self.currentFrameIndex];
    
    // 次のフレームのインデックスを計算
    self.currentFrameIndex = (self.currentFrameIndex + 1) % self.frames.count;
    
    // 次のフレームの遅延時間を取得
    float delay = [self.frameDelays[self.currentFrameIndex] floatValue];
    
    // タイマーを設定
    __weak typeof(self) weakSelf = self;
    self.animationTimer = [NSTimer scheduledTimerWithTimeInterval:delay repeats:NO block:^(NSTimer * _Nonnull timer) {
        [weakSelf animateNextFrame];
    }];
}

- (void)pauseAnimating {
    if (!self.isAnimating) return;
    
    self.isAnimating = NO;
    [self.animationTimer invalidate];
    self.animationTimer = nil;
}

- (void)resumeAnimating {
    if (self.isAnimating || self.frames.count == 0) return;
    
    self.isAnimating = YES;
    [self animateNextFrame];
}

- (void)stopAnimating {
    self.isAnimating = NO;
    [self.animationTimer invalidate];
    self.animationTimer = nil;
    self.currentFrameIndex = 0;
    if (self.frames.count > 0) {
        self.image = self.frames[0];
    }
}

- (void)dealloc {
    [self stopAnimating];
}




- (NSImage *)getFrameAtIndex:(NSInteger)index fromGIFAtURL:(NSURL *)gifURL {
    CGImageSourceRef source = CGImageSourceCreateWithURL((__bridge CFURLRef)gifURL, NULL);
    if (!source) return nil;
    
    if (index >= CGImageSourceGetCount(source)) {
        CFRelease(source);
        return nil;
    }
    
    CGImageRef imageRef = CGImageSourceCreateImageAtIndex(source, index, NULL);
    CFRelease(source);
    
    if (!imageRef) return nil;
    
    NSImage *image = [[NSImage alloc] initWithCGImage:imageRef size:NSZeroSize];
    CGImageRelease(imageRef);
    
    return image;
}




@end


#endif // _H_NONNON_MAC_GIFANIMATIONVIEW

