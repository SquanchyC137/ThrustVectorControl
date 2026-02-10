# Gesprächsprotokoll (2026-02-10)

## Analyse des Projekts
**User:** Kannst du meinen Ordner lesen und dir vorstellen, was das Ziel ist?
**Antigravity:** Ja. Es handelt sich um eine Teststation für eine Schubvektorsteuerung (Thrust Vector Control - TVC) mit ESP32-S3, MPU6050 und Servos.

## Fragen zur Umgebung
**User:** Brauche ich hier auch die Extensions, wie ich sie in VS Code nutze?
**Antigravity:** Nein. Ich kann den Code direkt lesen/schreiben. Du kannst deine lokale Umgebung (VS Code + PlatformIO) wie gewohnt zum Flashen nutzen.

## Onboarding Q&A
**1. Handlungsmöglichkeiten:**
Ich kann Code lesen/schreiben, Terminalbefehle ausführen, planen und recherchieren.

**2. Allgemeine Nutzung:**
Sei spezifisch, gib Kontext, arbeite iterativ.

**3. Nutzung im Projekt (Pair Programming):**
Du möchtest verstehen und selbst implementieren.
- Ich erkläre Code.
- Ich diskutiere Architektur.
- Ich mache Code-Reviews.
- Ich erstelle Gerüste (Skeletons).

**4. Subscriptions:**
Abhängig von der Plattform (Google Cloud / Gemini Code Assist).

**5. Zugriffsrechte:**
Vollzugriff auf den Workspace, Internet (Lesen), Terminal (User-Rechte). Kein Zugriff außerhalb des Workspaces ohne Erlaubnis.

**6. Grenzen:**
Keine Hardware-Interaktion (Kabel stecken), keine Echtzeit-Reaktion, keine visuelle Einsicht (Screenshots nötig).

**7. Modell:**
Gemini 1.5 Pro ist ideal für C++ und komplexe Logik.

**8. Geschwindigkeit:**
Nein, dank Kontext-Management bleibe ich effizient.

**9. Git Push:**
Ich pushe nur, was committed wurde.

**10. Arbeitsanweisung / Profil:**
Ja, über `.agent/rules.md` möglich.

**11. Projektdokumentation:**
Ja, kann ich erstellen (README, ARCHITECTURE.md).

**12. Vergleich zu Grok:**
Ich bin tiefer integriert (File-Access, Terminal).

## Multi-Device Nutzung
**User:** Kann ich dich synchron auf mehreren Geräten verwenden?
**Antigravity:** Nein, der Chat-Kontext ist lokal pro Session. Der Code wird über Git synchronisiert. Empfehlung: `.agent/rules.md` nutzen, um Anweisungen geräteübergreifend zu teilen.
