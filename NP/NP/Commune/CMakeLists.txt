cmake_minimum_required(VERSION 3.22.1)

project(Commune)

include_directories(headers)

set(SOURCES
    src/Carte.c
    src/Joueurs.c
    src/Client.c
    src/SendRcv.c
)

set(HEADERS
    headers/Carte.h
    headers/Client.h
    headers/inclusions.h
    headers/Joueurs.h
    headers/SendRcv.h
)

add_library(Commune ${SOURCES} ${HEADERS})
