/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#include "neofetch.h"

#include <ncurses.h>
#include <sys/utsname.h>
#include <unistd.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "../util/colours.h"
#include "../util/common.h"

constexpr const char* ASCII_ART = R"(
    P   O   L   Y

  .g8""8q.   .M"""bgd
.dP'    `YM.,MI    "Y
dM'      `MM`MMb.
MM        MM  `YMMNq.
MM.      ,MP.     `MM
`Mb.    ,dP'Mb     dM
  `"bmmd"'  P"Ybmmd"
)";

Neofetch::Neofetch()
    : Animation(TransitionState::Blank, TransitionState::Anything) {
    infoLines = {"ben @ polyphonic-1",
                 "==================",
                 "OS - polyOS",
                 "Kernel - 6.15.9-arch1-1",
                 "Shell - /bin/bash 5.3.3(1)",
                 "Display - Sony PVM-14L5",
                 "Resolution - 320x240 @ 60.08Hz",
                 "Terminal - alacritty",
                 "Font - scientifica",
                 "CPU - AMD Ryzen 5 8500G @ 5GHz",
                 "GPU - AMD Radeon HD 8490",
                 "Memory - 14.71GiB"};
    return;
    char* user = getenv("USER");
    std::string username = user ? user : "?";

    char hostname[256] = "";
    // TODO: error check
    gethostname(hostname, sizeof(hostname));
    std::string hostStr = hostname;

    std::string title = username + " @ " + hostStr;

    infoLines.push_back(title);
    infoLines.push_back(std::string(title.size(), '='));

    infoLines.push_back("OS - polyOS");

    struct utsname uts;
    if (uname(&uts) == 0) {
        infoLines.push_back(std::string("Kernel - ") + uts.release);
    } else {
        infoLines.push_back("Kernel - ?");
    }

    FILE* fp = popen("pacman -Qq | wc -l", "r");
    char pkgBuf[32] = "";
    int pkgCount = 0;
    if (fp && fgets(pkgBuf, sizeof(pkgBuf), fp)) {
        pkgCount = atoi(pkgBuf);
    }
    if (fp)
        pclose(fp);
    infoLines.push_back("Packages - " + std::to_string(pkgCount) + " (pacman)");

    std::string shell = getenv("SHELL") ? getenv("SHELL") : "?";
    std::string shellVer = "";
    if (shell.find("bash") != std::string::npos) {
        FILE* fp2 = popen("bash --version | head -1 | awk '{print $4}'", "r");
        char verBuf[64] = "";
        if (fp2 && fgets(verBuf, sizeof(verBuf), fp2)) {
            shellVer = verBuf;
            shellVer.erase(shellVer.find_last_not_of(" \n\r") + 1);
        }
        if (fp2)
            pclose(fp2);
    }
    infoLines.push_back("Shell - " + shell +
                        (shellVer.empty() ? "" : (" " + shellVer)));

    // Number of running processes
    FILE* fpProc = popen("ps ax | wc -l", "r");
    char procBuf[32] = "";
    int procCount = 0;
    if (fpProc && fgets(procBuf, sizeof(procBuf), fpProc)) {
        procCount = atoi(procBuf);
    }
    if (fpProc)
        pclose(fpProc);
    infoLines.push_back("Processes - " + std::to_string(procCount));

    FILE* fpUptime = fopen("/proc/uptime", "r");
    if (fpUptime) {
        double upSeconds = 0;
        if (fscanf(fpUptime, "%lf", &upSeconds) == 1) {
            int days = upSeconds / 86400;
            int hours = ((int)upSeconds % 86400) / 3600;
            int mins = ((int)upSeconds % 3600) / 60;
            char uptimeStr[64];
            if (days > 0)
                snprintf(uptimeStr, sizeof(uptimeStr), "Uptime - %dd %dh %dm",
                         days, hours, mins);
            else if (hours > 0)
                snprintf(uptimeStr, sizeof(uptimeStr), "Uptime - %dh %dm",
                         hours, mins);
            else
                snprintf(uptimeStr, sizeof(uptimeStr), "Uptime - %dm", mins);
            infoLines.push_back(uptimeStr);
        }
        fclose(fpUptime);
    }

    std::string displayInfo = "";
    FILE* fp3 = popen("xrandr | grep '*' | head -1 | awk '{print $1}'", "r");
    char dispBuf[64] = "";
    if (fp3 && fgets(dispBuf, sizeof(dispBuf), fp3)) {
        displayInfo = dispBuf;
        displayInfo.erase(displayInfo.find_last_not_of(" \n\r") + 1);
    }
    if (fp3)
        pclose(fp3);
    infoLines.push_back("Resolution - " +
                        (displayInfo.empty() ? "?" : displayInfo));

    infoLines.push_back("Font - scientifica");

    std::string cpuName = "";
    FILE* fp4 = fopen("/proc/cpuinfo", "r");
    if (fp4) {
        char line[256];
        while (fgets(line, sizeof(line), fp4)) {
            std::string l(line);
            if (l.find("model name") != std::string::npos) {
                auto pos = l.find(":");
                if (pos != std::string::npos) {
                    cpuName = l.substr(pos + 2);
                    cpuName.erase(cpuName.find_last_not_of(" \n\r") + 1);
                }
                break;
            }
        }
        fclose(fp4);
    }
    infoLines.push_back("CPU - " + (cpuName.empty() ? "?" : cpuName));

    // Kernel architecture
    infoLines.push_back(std::string("Arch - ") + uts.machine);

    // CPU core count
    int nproc = sysconf(_SC_NPROCESSORS_ONLN);
    infoLines.push_back("Cores - " + std::to_string(nproc));

    // IP Address
    FILE* fpIp = popen("hostname -I | awk '{print $1}'", "r");
    char ipBuf[64] = "";
    std::string ipStr = "";
    if (fpIp && fgets(ipBuf, sizeof(ipBuf), fpIp)) {
        ipStr = ipBuf;
        ipStr.erase(ipStr.find_last_not_of(" \n\r") + 1);
    }
    if (fpIp)
        pclose(fpIp);
    if (!ipStr.empty())
        infoLines.push_back("IP - " + ipStr);

    // Memory info
    FILE* fpMem = fopen("/proc/meminfo", "r");
    if (fpMem) {
        char line[256];
        while (fgets(line, sizeof(line), fpMem)) {
            std::string l(line);
            if (l.find("MemTotal") != std::string::npos) {
                auto pos = l.find(":");
                if (pos != std::string::npos) {
                    std::string memValue = l.substr(pos + 1);
                    memValue.erase(memValue.find_last_not_of(" \n\r") + 1);
                    long memKb = strtol(memValue.c_str(), nullptr, 10);
                    double memGiB = memKb / 1048576.0;
                    char memOut[64];
                    snprintf(memOut, sizeof(memOut), "%.2fGiB", memGiB);
                    infoLines.push_back(std::string("Memory - ") + memOut);
                }
                break;
            }
        }
        fclose(fpMem);
    }
}

void Neofetch::drawFrame(const AnimationContext& context) {
    int winHeight, winWidth;
    context.getDimensions(winHeight, winWidth);
    werase(context.window);

    // Split ASCII_ART into lines
    std::vector<std::string> artLines;
    {
        std::istringstream artStream(ASCII_ART);
        std::string line;
        while (std::getline(artStream, line)) {
            if (line.find_first_not_of(" \t\n\r") == std::string::npos) {
                artLines.push_back("");
            } else {
                artLines.push_back(line);
            }
        }
    }

    int artWidth = 0, artHeight = 0;
    getStringDimensions(ASCII_ART, artWidth, artHeight);

    int infoWidth = 0;
    int infoHeight = infoLines.size();
    for (const auto& line : infoLines) {
        if ((int)line.size() > infoWidth) {
            infoWidth = line.size();
        }
    }

    constexpr int COLOUR_BAR_WIDTH = 6;
    auto randomGradients = getAllRandomGradients(context.rng);
    int colourBarHeight = (int)randomGradients.size();
    int colourBarLen = GRADIENT_LENGTH * COLOUR_BAR_WIDTH;

    // Center the whole animation horizontally around the sum of ascii art width
    // and info width
    int totalWidth = artWidth + infoWidth + 4;  // 4 is the gap
    // int leftPad = (winWidth - totalWidth) / 2;
    // if (leftPad < 0)
    //     leftPad = 0;
    int leftPad = 2;
    int artColX = leftPad;
    // int infoColX = artColX + artWidth + 4;
    int infoColX = artColX + artWidth + 3;

    int infoAndBarHeight = infoHeight + colourBarHeight + 1;
    int blockHeight = std::max(artHeight, infoAndBarHeight);
    int blockTopPad = (winHeight - blockHeight) / 2;
    int artTopPad = blockTopPad + (blockHeight - artHeight) / 2;
    int infoTopPad = blockTopPad + (blockHeight - infoAndBarHeight) / 2;

    // Fade in ASCII art
    int fadeFrames = 12;
    for (int frame = 0; frame <= fadeFrames; ++frame) {
        float progress = (float)frame / fadeFrames;
        int linesToShow = std::round(progress * artLines.size());
        werase(context.window);
        for (int i = 0; i < linesToShow && (artTopPad + i) < winHeight; ++i) {
            mvwprintw(context.window, artTopPad + i, artColX, "%s",
                      artLines[i].c_str());
        }
        wrefresh(context.window);
        std::this_thread::sleep_for(
            std::chrono::milliseconds(MS_PER_SIXTEENTH_BEAT));
    }

    // Animate info text
    curs_set(TRUE);
    int infoX = infoColX;
    std::exponential_distribution<double> typingDelayDist(1.0 / 20.0);
    for (size_t i = 0;
         i < infoLines.size() && (infoTopPad + (int)i) < winHeight; ++i) {
        int lineY = infoTopPad + i;
        int lineX = infoX;
        const std::string& line = infoLines[i];
        for (size_t c = 0; c < line.size(); ++c) {
            mvwaddch(context.window, lineY, lineX + c, line[c]);
            wmove(context.window, lineY, lineX + c + 1);
            wrefresh(context.window);
            int delay = std::clamp(
                static_cast<int>(typingDelayDist(context.rng)), 10, 60);
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(MS_PER_DOUBLE_BEAT));
    curs_set(FALSE);

    // Animate gradient bars
    int gradY = infoTopPad + infoHeight + 1;
    int gradX = infoColX;
    for (int col = 0; col < colourBarLen; ++col) {
        for (size_t g = 0; g < randomGradients.size(); ++g) {
            int gradIdx = (GRADIENT_LENGTH > 0)
                              ? (col * GRADIENT_LENGTH) / colourBarLen
                              : 0;
            if (gradIdx >= GRADIENT_LENGTH)
                gradIdx = GRADIENT_LENGTH - 1;
            int colourPair = getInverseColourIndex(randomGradients[g], gradIdx);
            wattron(context.window, COLOR_PAIR(colourPair));
            mvwaddch(context.window, gradY + g, gradX + col, ' ');
            wattroff(context.window, COLOR_PAIR(colourPair));
        }
        wrefresh(context.window);
        std::this_thread::sleep_for(
            std::chrono::milliseconds(MS_PER_THIRTY_SECOND_BEAT));
    }
    std::this_thread::sleep_for(
        std::chrono::milliseconds(MS_PER_QUADRUPLE_BEAT * 60));

    // Fade out gradient bars
    for (int col = 0; col < colourBarLen; ++col) {
        for (size_t g = 0; g < randomGradients.size(); ++g) {
            mvwaddch(context.window, gradY + g, gradX + col, ' ');
        }
        wrefresh(context.window);
        std::this_thread::sleep_for(
            std::chrono::milliseconds(MS_PER_THIRTY_SECOND_BEAT));
    }
    wrefresh(context.window);
    std::this_thread::sleep_for(std::chrono::milliseconds(MS_PER_BEAT));
}
