#pragma once

namespace dsac::domain::details {

template <typename T, typename U>
auto cast(U const& u) -> T;

}  // namespace dsac::domain::details