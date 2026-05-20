/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** IMonoBehavior
*/

#ifndef IMONOBEHAVIOR_HPP_
#define IMONOBEHAVIOR_HPP_

#include "Engine.hpp"

namespace aot {
    class IMonoBehavior {
      public:
        virtual ~IMonoBehavior() = default;

        virtual void Start(Engine::Core &core) = 0;
        virtual void Update(Engine::Core &core) = 0;
        virtual void FixedUpdate(Engine::Core &core) = 0;
        virtual void Stop(Engine::Core &core) = 0;

      protected:
      private:
    };
}  // namespace aot

#endif /* !IMONOBEHAVIOR_HPP_ */
