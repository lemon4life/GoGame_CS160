#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <functional>

namespace Theme {
    const sf::Color BtnSuccess(50, 150, 50);   // Green
    const sf::Color BtnDanger(200, 50, 50);    // Red
    const sf::Color BtnPrimary(100, 100, 200); // Blue
    const sf::Color BtnDisabled(80, 80, 80);   // Grey
    const sf::Color TextNormal(255, 255, 255); // White
    const sf::Color Background(30, 30, 30);    // Dark Grey

    class Button {
    private:
        sf::RectangleShape m_shape;
        sf::Text m_text;

        // Helper to keep text centered when changed
        void centerText() {
            sf::FloatRect textRect = m_text.getLocalBounds();
            sf::Vector2f center = m_shape.getPosition();
            m_text.setOrigin(textRect.left + textRect.width / 2.0f,
                             textRect.top + textRect.height / 2.0f);
            m_text.setPosition(center);
        }

    public:
        // Simple Constructor
        Button(const sf::Font& font, const std::string& label, const sf::Vector2f& size,
               const sf::Vector2f& position, const sf::Color& color, int fontSize = 20)
        {
            m_shape.setSize(size);
            m_shape.setOrigin(size.x / 2.0f, size.y / 2.0f);
            m_shape.setPosition(position);
            m_shape.setFillColor(color);
            m_shape.setOutlineThickness(1);
            m_shape.setOutlineColor(sf::Color::White);

            m_text.setFont(font);
            m_text.setString(label);
            m_text.setCharacterSize(fontSize);
            m_text.setFillColor(sf::Color::White);

            // Auto-center text on init
            centerText();
        }

        // Returns true if clicked
        bool isClicked(const sf::Vector2i& mousePos) const {
            return m_shape.getGlobalBounds().contains(
                static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)
            );
        }

        void draw(sf::RenderWindow& window) {
            window.draw(m_shape);
            window.draw(m_text);
        }

        // Optional: visual hover effect
        void update(const sf::Vector2i& mousePos) {
            if (isClicked(mousePos)) {
                m_shape.setOutlineThickness(3); // Highlight on hover
            } else {
                m_shape.setOutlineThickness(1);
            }
        }

        // --- Setters for Dynamic Updates ---

        void setText(const std::string& text) {
            m_text.setString(text);
            centerText(); // Re-center the new text
        }

        void setFillColor(const sf::Color& color) {
            m_shape.setFillColor(color);
        }

        void setOutlineColor(const sf::Color& color) {
            m_shape.setOutlineColor(color);
        }
    };
}

