cmake_minimum_required(VERSION 3.10)
project(Program4 C)

set(CMAKE_C_STANDARD 99)

add_executable(keygen keygen.c)

add_executable(otp_enc otp_enc.c otplib.c)

add_executable(otp_enc_d otp_enc_d.c otplib.c)

add_executable(otp_dec otp_dec.c otplib.c)

add_executable(otp_dec_d otp_dec_d.c otplib.c)

