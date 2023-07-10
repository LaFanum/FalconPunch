#include <Game/Atlas.hpp>
#include <Game/Playstate.hpp>
#include <Game/Globals.hpp>
#include <SFML/Audio.hpp>
#include <vector>

sf::Music titleMusic;
sf::SoundBuffer confirmBuffer;
sf::Sound confirmSound;
sf::Text introText;
sf::Text titleText;
sf::Clock title_clock;

double title_BPM = 102.0;
double titleConfirmTime = -1.0;
double alpha = 0;

bool titleAlreadyPressEnter = false;
bool titlecanPressEnter = true;

int titleScreenState = 0;

std::pair<std::string, std::string> selectedText;
std::vector<std::pair<std::string, std::string>> funnyTexts = {
   {"A FULL GAME MAYBE??", "YEAAAAAAH"}
};

template <typename I>
I random_element(I begin, I end)
{
    const unsigned long n = std::distance(begin, end);
    const unsigned long divisor = (RAND_MAX + 1) / n;

    unsigned long k;
    do
    {
        k = std::rand() / divisor;
    } while (k >= n);

    std::advance(begin, k);
    return begin;
}

void title_init()
{
    titleAlreadyPressEnter = false;
    titlecanPressEnter = true;
    titleScreenState = 0;
    titleConfirmTime = -1.0;
    std::srand(std::time(0));
    selectedText = funnyTexts[std::rand() % funnyTexts.size()];
    introText.setFont(phantommuff);
    introText.setCharacterSize(64);
    titleMusic.openFromFile("assets/music/lol.ogg");
    confirmBuffer.loadFromFile("assets/sounds/confirmMenu.ogg");
    confirmSound.setBuffer(confirmBuffer);
    titleMusic.play();
    title_clock.restart();
}

void DrawIntroTextWithString(std::string string, double level = 0)
{
    introText.setString(string);
    introText.setPosition(win_size.x / 2, win_size.y / 3 + float(level * 64.0));
    introText.setOrigin(sf::Vector2f(introText.getLocalBounds().width, introText.getLocalBounds().height) / 2.f);
    window.draw(introText);
}

bool IntInRange(int x, int min, int max)
{
    return x >= min && x <= max;
}

void title_update(double delta)
{
    double time = title_clock.getElapsedTime().asSeconds();
    int beat = int(time / (60.0 / title_BPM));
    window.clear(sf::Color(0, 0, 0));

    // You can pretty much ignore whats happening at the bottom.

    if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) titleAlreadyPressEnter = false;

    if (titleScreenState < 1)
    {
        if (IntInRange(beat, 0, 3))
        {
            DrawIntroTextWithString("FALCONPUNCH BY");

            if (IntInRange(beat, 3, 3))
            {
                DrawIntroTextWithString("Rattus", 1);
                DrawIntroTextWithString("_DARK_", 2);
                DrawIntroTextWithString("Kunoguti", 3);
            }
        }
        else if (IntInRange(beat, 4, 7))
        {
            DrawIntroTextWithString("LITERALLY NOT ASSOCIATED");
            DrawIntroTextWithString("WITH", 1);
            if (IntInRange(beat, 7, 7))
                DrawIntroTextWithString("NEWGROUNDS", 2);
        }
        else if (IntInRange(beat, 8, 11))
        {
            DrawIntroTextWithString(selectedText.first);

            if (IntInRange(beat, 10, 11))
                DrawIntroTextWithString(selectedText.second, 1);
        }
        else if (IntInRange(beat, 12, 15))
        {
            DrawIntroTextWithString("FALCON");
            if (IntInRange(beat, 13, 15))
            {
                if (selectedText.second == "DEMENTIA")
                {
                    DrawIntroTextWithString("FALCON", 1);
                }
                else
                {
                    DrawIntroTextWithString("PUNCH", 1);
                }
            }
            if (IntInRange(beat, 14, 15))
            {
                if (selectedText.second == "DEMENTIA")
                {
                    DrawIntroTextWithString("FACLON", 2);
                }
                else
                {
                    DrawIntroTextWithString("DEMO", 2);
                }
            }
        }
        
        if (IntInRange(beat, 16, 16) || (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) && !titleAlreadyPressEnter))
        {
            titleAlreadyPressEnter = true;
            titleScreenState = 1;
            alpha = 255;
        }
    }

    if (titleScreenState == 1)
    {
     
    introText.setColor(sf::Color::White);
    titleText.setColor(sf::Color::White);
    titleText.setFont(phantommuff);
    titleText.setCharacterSize(64);
    titleText.setString("FalconPunch");
    titleText.setPosition(win_size.x/3, win_size.y/9);
    window.draw(titleText);

        sf::RectangleShape flash;
        flash.setSize(sf::Vector2f(win_size));
        flash.setFillColor(sf::Color(255, 255, 255, alpha));

        window.draw(flash);
        alpha -= delta * 128;
        alpha = std::max(0.0, alpha);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) && !titleAlreadyPressEnter && titlecanPressEnter)
        {
            alpha = 255;
            titleConfirmTime = time;
            titlecanPressEnter = false;
            titleAlreadyPressEnter = true;
            confirmSound.play();
        }

        if (titleConfirmTime > 0 && time - titleConfirmTime > 1 && fadeDir != 1)
        {
            fadeDir = 1;
            fadeProgress = -3;
        } else if (titleConfirmTime > 0 && time - titleConfirmTime > 1 && fadeDir == 1)
        {
            if (abs(fadeProgress) < 0.5) {
                titleMusic.stop();
                SetCurrentScene(&PlayState);
            }
        }
    }
}

Scene TitleState("Titlestate", title_init, title_update);
