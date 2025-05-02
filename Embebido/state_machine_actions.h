typedef void (*action)();
// Actions
void initialize();
void noScheduleSet();
void settingSchedule();
void awaitingTimer();
void moving();
void scanning();
void pillDetected();
void noPillDetected();
void doseSkipped();
void returnToHome();
void error();
void none();