#include "UserInterface.h"
#include "fonts/roboto_bold_8.h"
#include "fonts/roboto_bold_10.h"
#include "images/qr_code.h"

UserInterface::UIPage currentUIPage = UserInterface::UIPage::PAGE_ENABLED; // Current UI page state

void UserInterface::setup()
{
    Display.init(); // Initialize OLED
    Display.clear();
    Display.setBrightness(5);
    Display.invertDisplay();
    Display.setFont(Roboto_Bold_10);
    Display.drawString(50, 20, "Wait ah...");
    Display.display();
}

void UserInterface::renderUI()
{
    if (GlobalVariables::trolleyEnabled)
    {
        currentUIPage = UIPage::PAGE_ENABLED;
        displayEnabledPage();
    }
    else
    {
        currentUIPage = UIPage::PAGE_DISABLED;
        displayDisabledPage();
    }
}

// OLED dimensions:  (128 x 64), (width x height)
void UserInterface::displayEnabledPage()
{
    Display.clear();
    Display.setFont(Roboto_Bold_10);
    // QR code
    Display.drawFastImage(0, 0, QR_width, QR_height, qr_code_image); // or drawXbm()
    // Trolley name
    Display.setTextAlignment(TEXT_ALIGN_CENTER);
    Display.drawStringMaxWidth(QR_width + QR_xOffset, 0, 128 - QR_width, GlobalVariables::trolleyName);
    // Wi-Fi status
    Display.setTextAlignment(TEXT_ALIGN_CENTER);
    Display.drawStringMaxWidth(QR_width + QR_xOffset, 35, 128 - QR_width, String(GlobalVariables::bleClients) + String(" users connected."));
    Display.drawStringMaxWidth(QR_width + QR_xOffset, 20, 128 - QR_width, WiFi.status() == WL_CONNECTED ? "Online" : "Offline");
    // Display.drawStringMaxWidth(QR_width + QR_xOffset, 40, 128 - QR_width, "Location");
    Display.display();
}

void UserInterface::displayDisabledPage()
{
    Display.clear();
    Display.setFont(Roboto_Bold_10);
    Display.setTextAlignment(TEXT_ALIGN_CENTER);
    Display.drawStringMaxWidth(64, 10, 78, GlobalVariables::trolleyName);
    Display.drawString(60, 40, "This trolley is disabled");
    Display.display();
}