# MainWindow.cpp Conflict Guide

When merging branches that modified `project/ui/MainWindow.cpp`, conflicts typically appear in three areas:

1) **safe index parsing helpers (file top)**
   * Keep the `safeToInt(const QVariant&, bool&)` and `safeToInt(const QJsonValue&, bool&)` helpers near the top of the file.
   * These ensure QVariant/QJsonValue conversions fall back to `-1` instead of throwing or producing invalid conversions during appointment creation.

2) **Appointment creation (onCreateAppointment)**
   * Preserve the guarded lambda that reads the employee index from `Qt::UserRole` via `safeToInt` and verifies bounds before using it.
   * Reject creating an appointment when the selected employee index is negative or outside the current salon's employee list.

3) **JSON (de)serialization of appointments**
   * While saving, write `employeeIndex` and `serviceIndex` values alongside `startEpoch`, `duration`, `status`, and `price`.
   * While loading, call `safeToInt` on those index fields, reject negative/failed conversions, and bounds-check against `getEmployees()` and `getServices()` before constructing `Appointment` instances.

### Quick resolution steps

1. Prefer **our** side for the three hunks. That keeps the `safeToInt` helpers and the guarded lambda in `onCreateAppointment` that prevents the `toInt(-1)` compile error.
2. Ensure `safeToInt` helpers are defined once at the top of the file inside an unnamed namespace.
3. Confirm `onCreateAppointment` uses the guarded lambda pattern and returns early with a log message if the index is invalid.
4. Confirm `deserializeSalonFromJson` validates both indices with `safeToInt`, skips bad data, and sets active salon via `salonController.setActiveSalon` before returning.

### One-command escape hatch

If the conflict is only in `project/ui/MainWindow.cpp` and you just want the working version from this branch, you can run:

```bash
git checkout --ours project/ui/MainWindow.cpp
git add project/ui/MainWindow.cpp
```

That restores the version with `safeToInt`, the guarded lambda in `onCreateAppointment`, and the JSON validation logic that avoids the QVariant conversion error.

Following the above keeps the build green and prevents the `QVariant::toInt` conversion error that appears if conflict resolution drops the helper usage.
