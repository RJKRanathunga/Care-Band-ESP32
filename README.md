
---

# Care Band – ESP32 Device

The **Care Band ESP32 firmware** powers the wearable device for dementia patients.  
It provides **energy-efficient home detection, GPS fallback, and fall detection**.  
If the patient leaves home or experiences a fall, caregivers are notified through the mobile app.

---

## ✨ Features
- Learns and stores known Wi-Fi BSSIDs during setup.
- Checks every 5 minutes if connected to home Wi-Fi.
- Falls back to nearby BSSIDs if not directly connected.
- Uses GPS only if Wi-Fi checks fail (energy-efficient).
- **Fall detection support** with caregiver notification.
- Sends data to the server via Oracle HTTP → Render.
- Caregivers notified if the patient:
  - Might have left home (no Wi-Fi + no GPS).
  - Has truly left home (confirmed GPS).
  - Has experienced a fall (location shared).

---

## 🛠️ Tech Stack
- **Platform:** Arduino (C++ for ESP32)  
- **Modules:** ESP32 Wi-Fi + GPS module + GSM module
- **Relay:** Oracle HTTP (not included here)  
- **Backend:** Care Band Server (Flask on Render)

---

## 🚀 Setup
1. Clone this repository.
2. Replace the current server credentials with your own.
3. Flash the firmware to an ESP32 using Arduino IDE.
4. Ensure Oracle HTTP relay and Care Band Server are running.

---

## 📌 Project Status
This project was developed as a **Semester 2 Engineering Design Project** by 4 members.  
It is now closed and no longer actively maintained, but contributions are welcome.

---

## ⚠️ Notes
- Some hardcoded credentials may exist in code. Replace them before use.  
- The **Oracle HTTP relay code is not included**; users must set it up themselves.  

---

## 👥 Contributors
- University of Moratuwa – ENTC Department, Semester 2 Project  
- Team of 4 members (including this repo maintainer)

---

## 📜 License
This project is released under a permissive license:

- You may use, modify, and redistribute this code for personal, educational, or commercial purposes.  
- Attribution is not required.  
- However, you may **not re-publish this project as your own work without making substantial changes to the codebase**.  
- Any reuse should clearly distinguish derivative work from the original Care Band project.

