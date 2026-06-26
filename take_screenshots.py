#!/usr/bin/env python3
"""
Screenshot tool for VigilSense Dashboard
"""

import time
import subprocess
import sys

def take_screenshots():
    """Take screenshots of the dashboard pages"""
    
    try:
        from playwright.sync_api import sync_playwright
    except ImportError:
        print("Installing playwright...")
        subprocess.check_call([sys.executable, "-m", "pip", "install", "playwright", "--break-system-packages"])
        subprocess.check_call(["playwright", "install", "chromium"])
        from playwright.sync_api import sync_playwright
    
    with sync_playwright() as p:
        print("Starting browser...")
        browser = p.chromium.launch(headless=True)
        page = browser.new_page(viewport={'width': 1920, 'height': 1080})
        
        # Screenshot 1: Main Dashboard
        print("Taking screenshot of main dashboard...")
        page.goto('http://localhost:8080', wait_until='networkidle', timeout=30000)
        time.sleep(3)  # Wait for charts to load
        page.screenshot(path='screenshots/dashboard_main.png', full_page=False)
        print("✓ Saved: screenshots/dashboard_main.png")
        
        # Screenshot 2: Logs Page
        print("Taking screenshot of logs page...")
        page.goto('http://localhost:8080/logs', wait_until='networkidle', timeout=30000)
        time.sleep(2)
        page.screenshot(path='screenshots/dashboard_logs.png', full_page=True)
        print("✓ Saved: screenshots/dashboard_logs.png")
        
        browser.close()
        print("\n✅ All screenshots saved in 'screenshots/' directory")

if __name__ == "__main__":
    import os
    os.makedirs('screenshots', exist_ok=True)
    take_screenshots()

