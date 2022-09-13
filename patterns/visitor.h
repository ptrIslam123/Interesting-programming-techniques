#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>

/**
 * Идея простая — вместо того, чтобы объявлять поведение внутри класса, мы делегируем
 * это поведение некоторому внешнему объекту. При этом объект-делегат называется посетителем (Visitor),
 * и в нем должны быть объявлены методы посещения для каждого конкретного типа из иерархии. Экземпляр
 * такого объекта мне нравится называть глаголом, описывающим нужный эффект, тем самым подчеркивая,
 * что визитор — скорее поведение, отделенное от данных, чем полноценный объект
 * (makeSomething vs somethingMaker).
*/

namespace visitor {

class Visitor;

class Button;

class Label;

struct Element {
    virtual void accept(Visitor &visitor) = 0;

    virtual ~Element() = default;
};

struct Visitor {
    virtual void visit(Button *button) = 0;

    virtual void visit(Label *label) = 0;

    virtual ~Visitor() = default;
};

struct Widget : public Element {
    virtual void draw() const = 0;
};

class Button : public Widget {
public:
    explicit Button(const std::string &name) : Widget(), name_(name) {}

    virtual void draw() const override {
        std::cout << "Button::draw with name: " << name_ << std::endl;
    }

    virtual void accept(Visitor &visitor) override {
        visitor.visit(this);
    }

    const std::string &name() const {
        return name_;
    }

private:
    std::string name_;
};


class Label : public Widget {
public:
    explicit Label(const std::string &name) : Widget(), name_(name) {}

    virtual void draw() const override {
        std::cout << "Label::draw with name: " << name_ << std::endl;
    }

    virtual void accept(Visitor &visitor) override {
        visitor.visit(this);
    }

    const std::string &name() const {
        return name_;
    }

private:
    std::string name_;
};


class MakeUnblockingWidgets : public Visitor {
    virtual void visit(Button *button) override {
        std::cout << "make unblock button with name: " << button->name() << std::endl;
    }

    virtual void visit(Label *) override {
    }
};

class MakeVisibleWidgets : public Visitor {
    virtual void visit(Button *button) override {
        std::cout << "make visible button with name: " << button->name() << std::endl;
    }

    virtual void visit(Label *label) override {
        std::cout << "make visible label with name: " << label->name() << std::endl;
    }
};

static void exampleVisitor() {
    std::vector<std::unique_ptr<Element>> elements;
    elements.reserve(2);

    elements.push_back(std::make_unique<Button>("some button"));
    elements.push_back(std::make_unique<Label>("some label"));

    MakeVisibleWidgets makeVisibleWidgets;
    std::for_each(elements.begin(), elements.end(), [makeVisibleWidgets](auto &element) mutable {
        element->accept(makeVisibleWidgets);
    });
}

} // namespace visitor