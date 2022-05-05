#pragma once

/// Describe the content of a AD8556 packet
struct AD8556Body
{
  float reading; ///< [N], 4 bytes
};