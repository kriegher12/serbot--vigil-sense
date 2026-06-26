# Telegram Alert Setup Guide

## Overview
VigilSense can send real-time alerts to Telegram when people are detected by YOLOv8. This guide will help you set up Telegram notifications.

## Step 1: Create a Telegram Bot

1. Open Telegram and search for **@BotFather**
2. Start a chat and send `/newbot`
3. Follow the prompts to name your bot (e.g., "VigilSense Alert Bot")
4. BotFather will give you a **Bot Token** - save this!
   - Example: `123456789:ABCdefGHIjklMNOpqrsTUVwxyz`

## Step 2: Get Your Chat ID

### Method 1: Using @userinfobot
1. Search for **@userinfobot** on Telegram
2. Start a chat with it
3. It will reply with your Chat ID (a number like `123456789`)

### Method 2: Using @getidsbot
1. Search for **@getidsbot** on Telegram
2. Start a chat with it
3. It will show your Chat ID

### Method 3: Using API (Alternative)
1. Send a message to your bot
2. Visit: `https://api.telegram.org/bot<YOUR_BOT_TOKEN>/getUpdates`
3. Look for `"chat":{"id":123456789}` in the response

## Step 3: Configure VigilSense

### Option A: Edit app.py directly

Open `app.py` and update these lines:

```python
# Telegram Bot Configuration
TELEGRAM_BOT_TOKEN = "YOUR_BOT_TOKEN_HERE"  # From BotFather
TELEGRAM_CHAT_ID = "YOUR_CHAT_ID_HERE"      # Your Chat ID
TELEGRAM_ENABLED = True  # Enable Telegram alerts
```

### Option B: Use environment variables (Recommended)

Create a `.env` file in the dashboard directory:

```bash
TELEGRAM_BOT_TOKEN=your_bot_token_here
TELEGRAM_CHAT_ID=your_chat_id_here
TELEGRAM_ENABLED=true
```

Then update `app.py` to load from environment:

```python
import os
from dotenv import load_dotenv

load_dotenv()

TELEGRAM_BOT_TOKEN = os.getenv("TELEGRAM_BOT_TOKEN", "")
TELEGRAM_CHAT_ID = os.getenv("TELEGRAM_CHAT_ID", "")
TELEGRAM_ENABLED = os.getenv("TELEGRAM_ENABLED", "false").lower() == "true"
```

## Step 4: Test the Integration

1. Restart the dashboard:
   ```bash
   python3 app.py
   ```

2. Test the Telegram alert:
   ```bash
   curl http://localhost:8080/api/telegram/test
   ```

3. Check your Telegram - you should receive a test message!

## Step 5: Configure Alert Settings

### Alert Cooldown
To prevent spam, alerts are sent only when:
- Person count changes (0 → 1+ or 1+ → 0)
- At least 30 seconds have passed since last alert

To change the cooldown, edit `app.py`:
```python
alert_cooldown = 30  # Seconds between alerts
```

## Alert Messages

### When Person Detected:
```
🚨 Person Detected!

👥 Count: 2 person(s)
🕐 Time: 2024-11-06 09:30:45
📍 Location: VigilSense Monitoring Area

⚠️ Check live feed: http://10.120.83.221:8080
```

### When Area Cleared:
```
✅ Area Clear

👥 Count: 0 persons
🕐 Time: 2024-11-06 09:35:20
📍 Location: VigilSense Monitoring Area
```

## Troubleshooting

### No alerts received
1. Check if `TELEGRAM_ENABLED = True`
2. Verify bot token is correct
3. Verify chat ID is correct
4. Make sure you've sent at least one message to your bot
5. Check dashboard logs for errors

### Too many alerts
- Increase `alert_cooldown` value
- Check if person detection is too sensitive
- Adjust YOLO confidence threshold

### Bot not responding
- Verify bot token is valid
- Check internet connection on Raspberry Pi
- Ensure bot is not blocked
- Test with `/api/telegram/test` endpoint

## Security Notes

⚠️ **Important:**
- Never commit your bot token to Git!
- Add `.env` to `.gitignore`
- Use environment variables for production
- Rotate bot token if exposed

## Advanced: Multiple Recipients

To send to multiple chat IDs, modify `send_telegram_alert()`:

```python
def send_telegram_alert(message):
    chat_ids = ["CHAT_ID_1", "CHAT_ID_2", "CHAT_ID_3"]
    for chat_id in chat_ids:
        # Send to each chat ID
        ...
```

## API Endpoints

- `GET /api/telegram/test` - Test Telegram alert
- `GET /api/people` - Get person count and Telegram status

## Example Configuration

```python
# In app.py
TELEGRAM_BOT_TOKEN = "123456789:ABCdefGHIjklMNOpqrsTUVwxyz"
TELEGRAM_CHAT_ID = "987654321"
TELEGRAM_ENABLED = True
alert_cooldown = 30  # 30 seconds between alerts
```

## Support

For issues:
1. Check dashboard logs: `tail -f /tmp/vigil_dashboard.log`
2. Test Telegram API directly
3. Verify bot and chat ID are correct



