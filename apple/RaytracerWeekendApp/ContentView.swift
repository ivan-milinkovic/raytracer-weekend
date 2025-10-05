//
//  ContentView.swift
//  RaytracerWeekendApp
//
//  Created by Ivan Milinkovic on 27. 9. 2025.
//

import SwiftUI

struct ContentView: View {
    
    @ObservedObject var rc = renderController
    
    var body: some View {
        VStack {
            HStack {
                Button("Render") {
                    rc.render()
                }
                
                Spacer().frame(width: 20)
                
                // Crashes in combination with @Published on macOS Tahoe
                // Picker("", selection: $rc.sceneId) {
                //     ForEach(1..<10) { i in
                //         Text("\(i)").tag(i)
                //     }
                // }
                // .pickerStyle(.segmented)
                // .frame(width: 300)
                
                HStack {
                    ForEach(1..<10) { i in
                        Button("\(i)") {
                            rc.sceneId = i
                        }
                        .tint(i == rc.sceneId ? .blue : nil)
                    }
                }
            }
            
            Group {
                if rc.isRendering {
                    ProgressView(value: rc.progress, total: 1.0)
                } else {
                    Rectangle().stroke(.clear)
                }
            }
            .frame(height: 15)
            
            if let img = rc.cgImage {
                Image(img, scale: 1.0, label: Text(""))
                    .resizable(resizingMode: .stretch)
                    .aspectRatio(contentMode: .fit)
            } else {
                Rectangle()
                    .stroke(.gray)
            }
        }
        .disabled(rc.isRendering)
        .padding()
        .task {
            rc.setup()
            rc.render()
        }
        .onChange(of: rc.sceneId) { _, _ in
            rc.render()
        }
    }
}

#Preview {
    ContentView()
}
