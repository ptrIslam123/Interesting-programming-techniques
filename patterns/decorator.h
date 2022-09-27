#pragma once

#include <iostream>
#include <memory>
#include <vector>

namespace decorator {

struct Widget {
    virtual void draw() const = 0;
    virtual ~Widget() = default;
};

class Button : public Widget {
    virtual void draw() const override {
        std::cout << "Button::draw" << std::endl;
    }
};

class Label : public Widget {
    virtual void draw() const override {
        std::cout << "Label::draw" << std::endl;
    }
};


class Border : public Widget {
    virtual void draw() const override {
        std::cout << "Border::draw" << std::endl;
    }
};

class ButtonWithBorder : public Widget {
public:
    ButtonWithBorder(const Button &button, const Border &border) :
            Widget(), button_(button), border_(border) {}
private:
    virtual void draw() const override {
        const Widget &border = border_;
        const Widget &button = button_;

        border.draw();
        button.draw();
    }

    Button button_;
    Border border_;
};

static void exampleDecorator() {
    std::vector<std::unique_ptr<Widget>> widgets;
    widgets.reserve(2);

    widgets.push_back(std::make_unique<Button>());
    widgets.push_back(std::make_unique<Label>());
    widgets.push_back(std::make_unique<ButtonWithBorder>(Button(), Border()));

    std::for_each(widgets.cbegin(), widgets.cend(), [](const auto &widget){
        widget->draw();
    });
}

} // namespace decorator