add_executable(QtChatPresence
    main.cpp
    ui/LoginView.h
    ui/LoginView.cpp
    ui/ChatView.h
    ui/ChatView.cpp
    ui/widgets/MessageItem.h
    ui/widgets/MessageItem.cpp
)

target_link_libraries(QtChatPresence CppGameChatCore Qt5::Core Qt5::Widgets Qt5::Network)