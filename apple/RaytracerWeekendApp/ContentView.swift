//
//  ContentView.swift
//  RaytracerWeekendApp
//
//  Created by Ivan Milinkovic on 27. 9. 2025..
//

import SwiftUI

struct ContentView: View {
    
    @State var cgImage: CGImage?
    
    var body: some View {
        VStack {
            Button("Render") {
                render()
            }
            
            if let cgImage {
                Image(cgImage, scale: 1.0, label: Text(""))
                    .resizable(resizingMode: .stretch)
                    .aspectRatio(contentMode: .fit)
            } else {
                EmptyView()
            }
        }
        .padding()
    }
    
    func render() {
        Task.detached {
            rwmain()
            let img = getRawImage().pointee
            
            let w = Int(img.w)
            let h = Int(img.h)
            let pixelSize = Int(img.pixel_size)
            
            let cgImg = await Images.cgImageSRGB(img.bytes, w: w, h: h, pixelSize: pixelSize)
            
            await MainActor.run {
                cgImage = cgImg
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
            let ptr = getImage()!
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

#Preview {
    ContentView()
}
