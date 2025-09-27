//
//  RenderController.swift
//  RaytracerWeekend
//
//  Created by Ivan Milinkovic on 27. 9. 2025.
//

import Foundation
import Combine
import CoreGraphics

let renderController = RenderController()

func callbackFromRender(imgPtr: UnsafeMutablePointer<RawImage>) {
    renderController.handle(imgPtr: imgPtr)
}


class RenderController: ObservableObject {
    
    @Published var cgImage: CGImage?
    
    func setup() {
        rw_set_render_pass_callback(callbackFromRender)
    }
    
    func render() {
        Task.detached {
            rwmain()
            await self.handle(imgPtr: rw_get_raw_image())
        }
    }
    
    func handle(imgPtr: UnsafeMutablePointer<RawImage>) {
        let img = rw_get_raw_image().pointee
        
        let w = Int(img.w)
        let h = Int(img.h)
        let pixelSize = Int(img.pixel_size)
        
        let cgImg = Images.cgImageSRGB(img.bytes, w: w, h: h, pixelSize: pixelSize)
        
        DispatchQueue.main.async {
            self.cgImage = cgImg
        }
    }
    
    /*
     // Swift calls destructors on classes.
     // If a copy destructor is deleted, then the pointer doesn't have the .pointee
     // and cannot be casted or bound to the Image class from image.h
    func render() {
        Task.detached {
            rwmain()
            let ptr = rw_get_image()!
            let img = ptr.pointee
            let pixels = img.Pixels()!
            
            let w = Int(img.W())
            let h = Int(img.H())
            let pixelSize = Int(img.pixel_size)
            let pixelCount = w * h
            
            var pxbuff = [UInt8]()
            pxbuff.reserveCapacity(pixelCount * pixelSize)
            for i in 0..<pixelCount {
                let pixel = pixels[i]
                pxbuff.append(UInt8(pixel.X() * 255))
                pxbuff.append(UInt8(pixel.Y() * 255))
                pxbuff.append(UInt8(pixel.Z() * 255))
            }
            let cgImg = await Images.cgImageSRGB(&pxbuff, w: w, h: h, pixelSize: pixelSize)
            
            await MainActor.run {
                cgImage = cgImg
            }
        }
    }
     */
}
