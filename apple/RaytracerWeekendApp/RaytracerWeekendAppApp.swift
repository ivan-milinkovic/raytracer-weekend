//
//  RaytracerWeekendAppApp.swift
//  RaytracerWeekendApp
//
//  Created by Ivan Milinkovic on 27. 9. 2025..
//

import SwiftUI

@main
struct RaytracerWeekendAppApp: App {
    
    @NSApplicationDelegateAdaptor(AppDelegate.self) var appDelegate
    
    var body: some Scene {
        WindowGroup {
            ContentView()
        }
    }
}

class AppDelegate: NSObject, NSApplicationDelegate {
    func applicationShouldTerminateAfterLastWindowClosed(_ sender: NSApplication) -> Bool {
        true
    }
}
