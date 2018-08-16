#ifndef COPYRIGHT_APPLICATION_HPP
#define COPYRIGHT_APPLICATION_HPP

#include <QCoreApplication>

namespace cr
{
  class application final : public QCoreApplication
  {
  public:
    application(int argc, char* argv[]);
    application(const application&) = delete;
    application(application&&) = delete;
    application& operator=(const application&) = delete;
    application& operator=(application&&) = delete;
    ~application() noexcept = default;

  private:
    std::string m_copyright_text;
  };
}

#endif
