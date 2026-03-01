@echo off
:: 한글 깨짐 방지 및 UTF-8 설정
chcp 65001 > nul

echo [1/3] 깃 서브모듈(vcpkg 파일) 데이터 채우는 중...
git submodule update --init --recursive

echo [2/3] vcpkg 실행 파일(exe) 생성 중...
call .\vcpkg\bootstrap-vcpkg.bat

echo [3/3] 비주얼 스튜디오와 이 폴더 연결 중...
.\vcpkg\vcpkg.exe integrate install

echo 모든 세팅이 완료되었습니다! 이제 비주얼 스튜디오에서 빌드하세요.
pause