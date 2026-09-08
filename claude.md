@../Common/claude.md

# MiniClock2 프로젝트 컨텍스트

바탕화면에 떠 있는 시계 + 시간 기록(TimeList) 도구. 부팅 시 자동 실행된다.

- 프로젝트 루트: `D:\1.Projects_C++\MiniClock2`
- App 클래스 소스: `MiniClock2.cpp` / `MiniClock2.h`

---

## 자체 패치 (Self Patch)

2026-09-07 적용. 공통 모듈 `Common/system/CSCSelfPatch/SCSelfPatch.{h,cpp}` 를 쓰며,
`InitInstance` 앞부분에서 `m_self_patch.startup()`, `ExitInstance` 에서 `m_self_patch.shutdown()` 을 호출한다.

- 서버 폴더: `/download/tools/KoinoTools/MiniClock2` (`ss.linkmemine.com:443`)
- 실행한 그 자리에서 패치한다. 정해진 설치 폴더가 없으므로 exe 를 어디에 두든 바로가기·시작프로그램 등록이 그대로 유효하다.
- 부팅 시 자동 실행 등록 대상이다 (`register_startup = true`). 이 목록의 다른 도구들은 등록하지 않는다.

원리·배포 절차는 <https://koinodoc.atlassian.net/wiki/spaces/2HfRugOJ0HVK/pages/48562184> 참조.
공통 규칙은 `Common/claude.md` 의 "자체 패치(Self Patch)" 절.

**아직 서버에 올라가 있지 않다.** 첫 배포 때 위 폴더를 만들고 `filelist.lst` + `<파일명>.zip` 을 함께 올려야 한다.