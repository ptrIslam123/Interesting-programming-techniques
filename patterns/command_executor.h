#pragma once

#include <iostream>
#include <string>
#include <functional>
#include <stack>
#include <any>
#include <map>

static void modifyDocName(const std::string &doc, const std::string &newDoc) {
    std::cout << "Change doc: " << doc << " name on new doc name: " << newDoc << std::endl;
}

static void removeDoc(const std::string &doc) {
    std::cout << "Remove doc with name: " << doc << std::endl;
}

static void createDoc(const std::string &doc) {
    std::cout << "Create a new doc with name: " << doc << std::endl;
}

class CommandExecutor {
public:
    enum class CommandType {
        Create,
        Remove,
        Modify
    };

    typedef std::function<void(const std::any&)> Command;

    void execute(const Command &cmd, const std::any &data, const CommandType cmdType) {
        static std::map<CommandType, Command> redoCommandsMap = {
                {CommandType::Create, [](const std::any &data){
                    const auto docName = std::any_cast<std::string>(data);
                    removeDoc(docName);
                }},
                {CommandType::Remove, [](const std::any &data){
                    const auto docName = std::any_cast<std::string>(data);
                    createDoc(docName);
                }},
                {CommandType::Modify, [](const std::any &data){
                    typedef std::pair<std::string, std::string> TargetType;
                    const auto oldDocName = std::any_cast<TargetType>(data).first;
                    const auto newDocName = std::any_cast<TargetType>(data).second;
                    modifyDocName(newDocName, oldDocName);
                }}
        };

        auto redoCmd = redoCommandsMap.find(cmdType);
        if (redoCmd == redoCommandsMap.cend()) {
            throw std::runtime_error("can`t find redo command");
        }

        redoCommands_.push(std::make_pair(redoCmd->second, data));
        cmd(data);
    }

    void redo(std::size_t count) {
        while (!redoCommands_.empty() && count != 0) {
            auto redoCmd = redoCommands_.top();
            (redoCmd.first)(redoCmd.second);
            redoCommands_.pop();
            --count;
        }
    }

private:
    typedef std::pair<Command, std::any> RedoCommand;
    std::stack<RedoCommand> redoCommands_;
};


static void exampleUsingCommandExecutor() {
    CommandExecutor executor;

    std::any data1 = std::string("file1.doc");
    executor.execute([](const std::any &data){
        createDoc(std::any_cast<std::string>(data));
    }, data1, CommandExecutor::CommandType::Create);

    std::any data2 = std::string("file2.doc");
    executor.execute([](const std::any &data){
        createDoc(std::any_cast<std::string>(data));
    }, data2, CommandExecutor::CommandType::Create);

    std::any data3 = std::string("file1.doc");
    executor.execute([](const std::any &data){
        removeDoc(std::any_cast<std::string>(data));
    }, data3, CommandExecutor::CommandType::Remove);

    std::any data4 = std::make_pair(std::string("file2.doc"), std::string("file3.doc"));
    executor.execute([](const std::any &data){
        typedef std::pair<std::string, std::string> TargetType;
        const auto oldDocName = std::any_cast<TargetType>(data).first;
        const auto newDocName = std::any_cast<TargetType>(data).second;
        modifyDocName(oldDocName, newDocName);
    }, data4, CommandExecutor::CommandType::Modify);

    std::cout << "===========================" << std::endl;
    executor.redo(4);
}
