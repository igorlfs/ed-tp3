#pragma once

#include <iostream>
#define avisoAssert(e, m)                                                      \
    ((void)((e) ? 0 : __avisoassert(#e, __FILE__, __LINE__, m)))
#define __avisoassert(e, file, line, m)                                        \
    (std::cerr << file ":" << line << ": Aviso:"                               \
               << "'" << e << "' - " << m << '\n',                             \
     0)
#define erroAssert(e, m)                                                       \
    ((void)((e) ? 0 : __erroassert(#e, __FILE__, __LINE__, m)))
#define __erroassert(e, file, line, m)                                         \
    (std::cerr << file ":" << line << ": Erro:"                                \
               << "'" << e << "' - " << m << '\n',                             \
     abort(), 0)
