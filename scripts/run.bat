@echo off

for /r "." %%a in (build\bin\*_seq.exe) do (
    echo -------------------------------------
    echo %%~na
    echo -------------------------------------
    %%~fa --gtest_repeat=10
)

for /r "." %%a in (build\bin\*_omp.exe) do (
    echo -------------------------------------
    echo %%~na
    echo -------------------------------------
    %%~fa --gtest_repeat=10
)

for /r "." %%a in (build\bin\*_tbb.exe) do (
    echo -------------------------------------
    echo %%~na
    echo -------------------------------------
    %%~fa --gtest_repeat=10
)

for /r "." %%a in (build\bin\*_std.exe) do (
    echo -------------------------------------
    echo %%~na
    echo -------------------------------------
    %%~fa --gtest_repeat=10
)

for /r "." %%a in (build\bin\*_mpi.exe) do (
    echo -------------------------------------
    echo %%~na
    echo -------------------------------------
    for /l %%x in (1, 1, 1) do (
        "C:\Program Files\Microsoft MPI\Bin\mpiexec.exe" -np 4 %%~fa || exit /b 1
    )
)


for /r "." %%a in (build\bin\*_Kryukov_number_of_alternating_char.exe) do (
    echo -------------------------------------
    echo %%~na
    echo -------------------------------------
    for /l %%x in (1, 1, 1) do (
        "C:\Users\HP-PC\Desktop\лаба 1 mpi\pp_2020_autumn_engineer\modules\task_1\Kryukov_num_of_altern_char\Kryukov_number_of_alternating_char\DebugKryukov_number_of_alternating_char.exe" -np 4 %%~fa || exit /b 1
    )
)

