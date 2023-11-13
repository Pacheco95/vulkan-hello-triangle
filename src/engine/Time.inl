template <typename Precision>
Precision Time::deltaTime() {
  auto now = high_resolution_clock::now();
  auto deltaTime = duration_cast<duration<Precision>>(now - lastTime);
  lastTime = now;
  return deltaTime.count();
}
