add_library(CppGameChatCore
    models/User.h
    models/Message.h
    models/Presence.h
    
    services/IFirestoreClient.h
    services/FirestoreClient.h
    services/FirestoreClient.cpp
    services/PresenceService.h
    services/PresenceService.cpp
    services/ChatService.h
    services/ChatService.cpp
    services/FriendsService.h
    services/FriendsService.cpp
    
    utils/Config.h
    utils/Config.cpp
    utils/Retry.h
    utils/Retry.cpp
    utils/Clock.h
    utils/Clock.cpp
    
    repo/FirestoreMapper.h
    repo/FirestoreMapper.cpp
)

target_link_libraries(CppGameChatCore Qt5::Core Qt5::Network)
target_include_directories(CppGameChatCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})