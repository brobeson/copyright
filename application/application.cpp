#include "application.hpp"
#include <QCommandLineParser>
#include <QDebug>
#include <fstream>
#include <iostream>
#include <string_view>

namespace cr
{
    namespace
    {
        // NOLINTNEXTLINE(google-runtime-references)
        // passing by non-const-reference is the only way to modify a QCommandLineParser
        void setup_command_parser(QCommandLineParser& parser)
        {
            // parser.addPositionalArgument(
            //    QObject::tr("command"),
            //    QObject::tr("One of 'check, add, or replace'."),
            //    QObject::tr("command"));

            parser.addHelpOption();
            parser.addVersionOption();
            parser.setApplicationDescription(
                QObject::tr("A tool to maintain copyright information in software source files."));

            parser.addOptions({{QObject::tr("copyright_file"),
                                QObject::tr("The file that contains the copyright text."),
                                "copyright file"}});

            parser.addPositionalArgument(QObject::tr("source file"),
                                         QObject::tr("The source file to anaylize."),
                                         QObject::tr("<source file>"));
        }

        std::string read_copyright_file(const QString& file_path)
        {
            /// \todo eliminate the std::string conversion
            std::ifstream file(file_path.toStdString().c_str());
            if (file.fail())
            {
                qCritical() << "unable to open file" << file_path << "for reading";
                std::exit(EXIT_FAILURE);
            }
            return std::string{std::istreambuf_iterator<char>{file},
                               std::istreambuf_iterator<char>{}};
        }

        std::ostream& operator<<(std::ostream& stream, const QtMsgType type)
        {
            constexpr std::string_view RED{"\033[0;31m"};
            constexpr std::string_view BLUE{"\033[0;34m"};
            constexpr std::string_view YELLOW{"\033[0;33m"};
            constexpr std::string_view NO_COLOR{"\033[0m"};

            switch (type)
            {
                case QtDebugMsg:
                    stream << BLUE << "debug" << NO_COLOR << "    ";
                    break;
                case QtInfoMsg:
                    stream << "info     ";
                    break;
                case QtWarningMsg:
                    stream << YELLOW << "warning" << NO_COLOR << "  ";
                    break;
                case QtCriticalMsg:
                case QtFatalMsg:
                    stream << RED << "error" << NO_COLOR << "    ";
                    break;
                default:
                    stream << YELLOW << "unknown" << NO_COLOR << "  ";
            };
            return stream;
        }

        std::ostream& operator<<(std::ostream& stream, const QMessageLogContext& context)
        {
            if (context.file != nullptr)
            {
                stream << "(" << context.file << ":" << context.line << ") ";
            }
            return stream;
        }

        void format_log_message(const QtMsgType type,
                                const QMessageLogContext& context,
                                const QString& message)
        {
            if (type == QtInfoMsg)
            {
                std::cout << type << context << message.toStdString() << std::endl;
            }
            else
            {
                std::cerr << type << context << message.toStdString() << std::endl;
            }
        }
    } // namespace

    application::application(int argc, char* argv[]) : QCoreApplication(argc, argv)
    {
        qInstallMessageHandler(format_log_message);
        setApplicationName(tr("Copyright"));
        setApplicationVersion(tr("1.0.0"));
        setOrganizationDomain("github.com/brobeson/copyright");
        setOrganizationName("brobeson");

        QCommandLineParser parser;
        setup_command_parser(parser);
        parser.process(*this);
        if (!parser.isSet("copyright_file"))
        {
            qCritical() << "The copyright file is required.";
            std::cerr << parser.helpText().toStdString();
            std::exit(EXIT_FAILURE);
        }
        if (parser.positionalArguments().empty())
        {
            qCritical() << "The source file to check is required.";
            std::cerr << parser.helpText().toStdString();
            std::exit(EXIT_FAILURE);
        }

        m_copyright_text = read_copyright_file(parser.value("copyright_file"));
    }
} // namespace cr
