#pragma once

namespace vision {
namespace hooks {
namespace winapi {

// MessageBox hook functions
bool install_message_box_hook();
bool remove_message_box_hook();

} // namespace winapi
} // namespace hooks
} // namespace vision
