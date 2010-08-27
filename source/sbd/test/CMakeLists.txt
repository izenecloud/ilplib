ADD_EXECUTABLE(t_sbd t_sbd.cpp)
ADD_EXECUTABLE(t_trainer t_trainer.cpp)
ADD_EXECUTABLE(t_collocation t_collocation.cpp)
ADD_EXECUTABLE(t_language t_language.cpp)
ADD_EXECUTABLE(t_token t_token.cpp)
ADD_EXECUTABLE(t_base t_base.cpp)
ADD_EXECUTABLE(t_regex t_regex.cpp)
ADD_EXECUTABLE(t_common t_common.cpp)

TARGET_LINK_LIBRARIES(
t_sbd ${LIBS}
)
TARGET_LINK_LIBRARIES(
t_trainer ${LIBS}
)
TARGET_LINK_LIBRARIES(
t_collocation ${LIBS}
)
TARGET_LINK_LIBRARIES(
t_language ${LIBS}
)
TARGET_LINK_LIBRARIES(
t_token ${LIBS}
)
TARGET_LINK_LIBRARIES(
t_base ${LIBS}
)
TARGET_LINK_LIBRARIES(
t_regex ${LIBS}
)
TARGET_LINK_LIBRARIES(
t_common ${LIBS}
)
