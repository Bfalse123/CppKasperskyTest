#include <cstdint>
#include <string>

namespace Client {
class Client {
   public:
    Client(const std::string& host, uint32_t port);
    void Send(const std::string& data) const;
    std::string Read() const;
    ~Client();

   private:
    int32_t socket;
};
}  // namespace Client