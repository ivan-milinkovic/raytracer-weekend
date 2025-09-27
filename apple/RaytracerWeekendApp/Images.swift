import Foundation
import CoreGraphics

class Images {
    
    static func cgImageSRGB(_ px: UnsafeRawPointer, w: Int, h: Int, pixelSize: Int) -> CGImage {
        let pixelData = Data(bytes: px, count: w * h * pixelSize)
        return cgImageSRGB(pixelData: pixelData, w: w, h: h, pixelSize: pixelSize)
    }
    
    static func cgImageSRGB(pixelData: Data, w: Int, h: Int, pixelSize: Int) -> CGImage {
        let cgDataProvider = CGDataProvider(data: pixelData as NSData)!
        let cgImage = CGImage(width: w,
                              height: h,
                              bitsPerComponent: 8,
                              bitsPerPixel: pixelSize*8,
                              bytesPerRow: w*pixelSize,
                              space: CGColorSpace(name: CGColorSpace.sRGB)!,
                              bitmapInfo: CGBitmapInfo(rawValue: CGImageAlphaInfo.none.rawValue),
                              provider: cgDataProvider,
                              decode: nil,
                              shouldInterpolate: false,
                              intent: CGColorRenderingIntent.defaultIntent)!
        return cgImage
    }
}
