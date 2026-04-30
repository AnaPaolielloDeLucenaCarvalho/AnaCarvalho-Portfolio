#ifndef COMMAND_H
#define COMMAND_H

namespace portfolio
{
    class Command
    {
    public:
        virtual ~Command() = default;
        virtual void Execute(float deltaTime) = 0;
    };
}

#endif