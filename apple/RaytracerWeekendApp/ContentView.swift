//
//  ContentView.swift
//  RaytracerWeekendApp
//
//  Created by Ivan Milinkovic on 27. 9. 2025..
//

import SwiftUI

struct ContentView: View {
    
    @ObservedObject var rc = renderController
    
    var body: some View {
        VStack {
            Button("Render") {
                rc.render()
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
        .padding()
        .task {
            rc.setup()
            rc.render()
        }
    }
}

#Preview {
    ContentView()
}
