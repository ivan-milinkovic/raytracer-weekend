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

func renderProgressCallback(progress: Double) {
    DispatchQueue.main.async {
        renderController.progress = progress
    }
}

func setup_callbacks() {
    rw_set_render_pass_callback(callbackFromRender)
    rw_set_render_progress_callback(renderProgressCallback)
}

final class RenderController: ObservableObject {
    
    @Published var sceneId: Int = 1
    @Published private(set) var cgImage: CGImage?
    @Published private(set) var isRendering: Bool = false
    @Published fileprivate(set) var progress: Double = 0
    
    private let renderQueue = DispatchQueue.init(label: "render-queue", qos: .userInitiated)
    var copyPxPtr: UnsafeMutableRawPointer?
    
    func setup() {
        setup_callbacks()
    }
    
    func render() {
        if isRendering { return }
        isRendering = true
        progress = 0.0
        cgImage = nil
        let sceneId: Int32 = Int32(self.sceneId)
        copyPxPtr?.deallocate()
        copyPxPtr = nil
        
        renderQueue.async { [sceneId] in
            rwmain(sceneId)
            self.handle(imgPtr: rw_get_raw_image(), isFinal: true)
            DispatchQueue.main.async {
                self.isRendering = false
            }
        }
    }
    
    func handle(imgPtr: UnsafeMutablePointer<RawImage>, isFinal: Bool = false) {
        let rawImg = rw_get_raw_image().pointee
        let bytes = rawImg.bytes!
        let w = Int(rawImg.w)
        let h = Int(rawImg.h)
        let pixelSize = Int(rawImg.pixel_size)
        let byteCount = w * h * pixelSize
        
        // avoids copying large buffers on each update
        // creates one at the beginning and destroys it at the end of render
        if copyPxPtr == nil {
            copyPxPtr = UnsafeMutableRawPointer.allocate(byteCount: byteCount, alignment: 1)
        }
        
        let imgBytesRawPtr = UnsafeMutableRawPointer(mutating: bytes)
        copyPxPtr!.copyMemory(from: imgBytesRawPtr, byteCount: byteCount)
        
        autoreleasepool {
            // let cgImg = Images.cgImageSRGB(bytes, w: w, h: h, pixelSize: pixelSize)
            let data: Data
            if isFinal {
                data = Data(bytes: copyPxPtr!, count: w * h * pixelSize)
                copyPxPtr?.deallocate()
                copyPxPtr = nil
            } else {
                data = Data.init(bytesNoCopy: copyPxPtr!, count: w * h * pixelSize, deallocator: .none)
            }
            let cgImg = Images.cgImageSRGB(pixelData: data, w: w, h: h, pixelSize: pixelSize)
            
            DispatchQueue.main.async {
                self.cgImage = cgImg
            }
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
