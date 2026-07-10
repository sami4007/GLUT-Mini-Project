#include <iostream>
#include <cmath>
#include <windows.h>
#include <GL/glut.h>

float carX = 100;
float carY = 40;
float saluteAngle = 0.0f;   // current salute angle
int saluteDir = 1;          // 1 = raising arm, -1 = lowering
const int numGrass = 80;    // number of grass blades
float grassX[numGrass];     // X positions
float grassH[numGrass];     // Heights
float swayOffset[numGrass]; // phase offsets
float swayAngle = 0.0f;     // animation angle
float nightOpacity = 0.0f;
float sunX = 100.0f;             // current sun X-position (starts left)
float sunY = 720.0f;             // constant Y-position for horizontal path
float sunSpeed = 1.0f;           // speed of sun movement
const float SUN_MIN_X = 100.0f;  // left edge
const float SUN_MAX_X = 1100.0f; // right edge
const int NUM_CARS = 2;

struct Clloud {
    float x, y;
    float size;
    float speed;
 };

 Clloud clouds[] = {
    //{200, 700, 30, 0.5f},
    {500, 650, 40, 0.3f},
    {800, 720, 35, 0.4f},
    {300, 750, 25, 0.6f},
    {1000, 680, 45, 0.35f}
 };
const int numClouds = sizeof(clouds)/sizeof(clouds[0]);

 struct Car {
    float x, y;       // position
    float speed;      // movement speed
};
 // number of cars
Car cars[NUM_CARS] = {
    {100.0f, 40.0f, 1.0f},
    {600.0f, 40.0f, 0.8f}
};

// OBJECT FUNCTIONS
// ============================
void drawGreenView() {
    // ---------- Background polygon (greenish-white) ----------
    glColor3f(0.35f, 0.55f, 0.35f); // muted green mixed with white
    glBegin(GL_POLYGON);
        glVertex2f(0.0f, 550.0f);
        glVertex2f(1200.0f, 550.0f);
        glVertex2f(1200.0f, 135.0f);
        glVertex2f(0.0f, 135.0f);
    glEnd();
}

void initGrass() {
    for (int i = 0; i < numGrass; i++) {
        grassX[i] = i * (1200.0f / numGrass);      // spread evenly
        grassH[i] = 12 + rand() % 12;              // random height (12–24)
        swayOffset[i] = (rand() % 100) / 20.0f;    // random sway offset
    }
}

void drawGrass() {
    // Soil/ground polygon
    glColor3f(0.35f, 0.55f, 0.35f); // dark green base
    glBegin(GL_POLYGON);
        glVertex2f(0, 135);
        glVertex2f(1200, 135);
        glVertex2f(1200, 151);
        glVertex2f(0, 151);
    glEnd();

    // Grass blades
    glColor3f(0.05f, 0.2f, 0.05f); // grass green
    glLineWidth(2.0f);

    for (int i = 0; i < numGrass; i++) {
        float baseX = grassX[i];
        float baseY = 135;

        float bladeHeight = grassH[i];
        float sway = sin(swayAngle + swayOffset[i]) * 4.0f; // unique sway

        glBegin(GL_LINES);
            glVertex2f(baseX, baseY);
            glVertex2f(baseX + sway, baseY + bladeHeight);
        glEnd();
    }

    glLineWidth(1.0f); // reset
}

void drawCircle(float cx, float cy, float r, int num_segments = 100) {
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(cx, cy); // center
        for (int i = 0; i <= num_segments; i++) {
            float angle = 2.0f * M_PI * i / num_segments;
            float x = r * cos(angle);
            float y = r * sin(angle);
            glVertex2f(cx + x, cy + y);
        }
    glEnd();
}

void drawTree(float x, float y) {
    // x, y = bottom center of the trunk

    // ----- Tree trunk -----
    glColor3f(0.55f, 0.27f, 0.07f); // brown
    glBegin(GL_POLYGON);
        glVertex2f(x - 5, y);      // bottom-left
        glVertex2f(x + 5, y);      // bottom-right
        glVertex2f(x + 5, y + 20); // top-right
        glVertex2f(x - 5, y + 20); // top-left
    glEnd();

    // ----- Tree foliage (overlapping circles) -----
    glColor3f(0.0f, 0.39f, 0.0f); // green
    drawCircle(x, y + 40, 20, 50);   // middle
    drawCircle(x - 15, y + 35, 18, 50); // left
    drawCircle(x + 15, y + 35, 18, 50); // right
    drawCircle(x, y + 55, 18, 50);   // top
}

void drawCar(float x, float y) {
    // ==== Car body ====
    glColor3f(0.8f, 0.0f, 0.0f);
    glBegin(GL_POLYGON);
        glVertex2f(x, y);
        glVertex2f(x + 160, y);
        glVertex2f(x + 160, y + 40);
        glVertex2f(x, y + 40);
    glEnd();

    // ==== Car roof ====
    glColor3f(0.9f, 0.9f, 0.9f);
    glBegin(GL_POLYGON);
        glVertex2f(x + 30, y + 40);
        glVertex2f(x + 130, y + 40);
        glVertex2f(x + 110, y + 65);
        glVertex2f(x + 50, y + 65);
    glEnd();

    // ==== Car windows ====
    glColor3f(0.3f, 0.6f, 0.9f);
    glBegin(GL_POLYGON);
        glVertex2f(x + 40, y + 40);
        glVertex2f(x + 120, y + 40);
        glVertex2f(x + 105, y + 60);
        glVertex2f(x + 55, y + 60);
    glEnd();

    // ==== Wheels ====
    for (int i = 0; i < 2; i++) {
        float cx = x + 45 + i * 90;
        float cy = y - 10;

        glColor3f(0.0f, 0.0f, 0.0f); // tire
        drawCircle(cx, cy, 18);

        glColor3f(0.7f, 0.7f, 0.7f); // rim
        drawCircle(cx, cy, 9);
    }

    // ==== Headlights ====
    glColor3f(1.0f, 1.0f, 0.3f);
    glBegin(GL_POLYGON);
        glVertex2f(x + 155, y + 10);
        glVertex2f(x + 160, y + 10);
        glVertex2f(x + 160, y + 25);
        glVertex2f(x + 155, y + 25);
    glEnd();

    // ==== Taillights ====
    glColor3f(0.8f, 0.0f, 0.0f);
    glBegin(GL_POLYGON);
        glVertex2f(x, y + 10);
        glVertex2f(x + 5, y + 10);
        glVertex2f(x + 5, y + 25);
        glVertex2f(x, y + 25);
    glEnd();
}

void drawCloud(float x, float y, float size) {
    glColor3f(0.95, 0.95, 0.98);
    int segments = 20;
    float r = size;

    // Center circle
    glBegin(GL_POLYGON);
    for (int i = 0; i < segments; i++) {
        float angle = 2.0f * 3.14159f * i / segments;
        glVertex2f(x + cos(angle) * r, y + sin(angle) * r);
    }
    glEnd();

    // Left circle
    glBegin(GL_POLYGON);
    for (int i = 0; i < segments; i++) {
        float angle = 2.0f * 3.14159f * i / segments;
        glVertex2f(x - r*0.7f + cos(angle) * r*0.8f, y + sin(angle) * r*0.8f);
    }
    glEnd();

    // Right circle
    glBegin(GL_POLYGON);
    for (int i = 0; i < segments; i++) {
        float angle = 2.0f * 3.14159f * i / segments;
        glVertex2f(x + r*0.7f + cos(angle) * r*0.9f, y + sin(angle) * r*0.9f);
    }
    glEnd();

    // Top circle
    glBegin(GL_POLYGON);
    for (int i = 0; i < segments; i++) {
        float angle = 2.0f * 3.14159f * i / segments;
        glVertex2f(x + cos(angle) * r*0.7f, y + r*0.7f + sin(angle) * r*0.7f);
    }
    glEnd();
}

void drawSun(float x, float y, float radius) {
    // Sun color
    glColor3f(1.0f, 0.9f, 0.0f); // bright yellow
    int num_segments = 100;

    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y); // center
        for (int i = 0; i <= num_segments; i++) {
            float angle = 2.0f * M_PI * i / num_segments;
            float dx = radius * cos(angle);
            float dy = radius * sin(angle);
            glVertex2f(x + dx, y + dy);
        }
    glEnd();
}

void drawSky() {
    // Sky background
    glColor3f(0.529, 0.808, 0.922);
    glBegin(GL_POLYGON);
        glVertex2f(0, 550);
        glVertex2f(1200, 550);
        glVertex2f(1200, 800);
        glVertex2f(0, 800);
    glEnd();

    // Draw all clouds
    for (int i = 0; i < numClouds; i++) {
        drawCloud(clouds[i].x, clouds[i].y, clouds[i].size);
    }


    // Draw Sun using animated sunY
    drawSun(sunX, sunY, 50);  // Use sunX variable
}

void drawStreet() {
    // Street background
    glColor3f(0.25f, 0.25f, 0.25f); // Dark grey asphalt
    glBegin(GL_POLYGON);
        glVertex2f(0, 0);
        glVertex2f(1200, 0);
        glVertex2f(1200, 135);
        glVertex2f(0, 135);
    glEnd();

    // White stripes (middle dashed line)
    glColor3f(1.0f, 1.0f, 1.0f); // White
    for (int x = 50; x < 1200; x += 200) {  // Repeated every 200px
        glBegin(GL_POLYGON);
            glVertex2f(x, 60);
            glVertex2f(x + 100, 60);
            glVertex2f(x + 100, 75);
            glVertex2f(x, 75);
        glEnd();
    }

    // Yellow border line (bottom)
    glColor3f(1.0f, 1.0f, 0.0f); // Yellow
    glBegin(GL_POLYGON);
        glVertex2f(0, 10);
        glVertex2f(1200, 10);
        glVertex2f(1200, 20);
        glVertex2f(0, 20);
    glEnd();

    // Yellow border line (top)
    glBegin(GL_POLYGON);
        glVertex2f(0, 115);
        glVertex2f(1200, 115);
        glVertex2f(1200, 125);
        glVertex2f(0, 125);
    glEnd();
}

void drawLeftPillars() {
    glColor3f(0.82f, 0.82f, 0.82f);
    // Polygon 1
    glBegin(GL_POLYGON);
        glVertex2f(204.1f, 151.9f);
        glVertex2f(182.1f, 151.9f);
        glVertex2f(173.1f, 180.8f);
        glVertex2f(192.7f, 180.8f);
    glEnd();

    // Polygon 2
    glBegin(GL_POLYGON);
        glVertex2f(192.7f, 180.8f);
        glVertex2f(173.1f, 180.8f);
        glVertex2f(174.5f, 230.4f);
        glVertex2f(193.4f, 230.1f);
    glEnd();

    // Polygon 3
    glBegin(GL_POLYGON);
        glVertex2f(174.5f, 230.4f);
        glVertex2f(193.4f, 230.1f);
        glVertex2f(200.0f, 260.0f);
        glVertex2f(173.9f, 250.9f);
    glEnd();

    // Polygon 4
    glBegin(GL_POLYGON);
        glVertex2f(150.9f, 242.8f);
        glVertex2f(144.1f, 260.5f);
        glVertex2f(193.8f, 277.5f);
        glVertex2f(200.0f, 260.0f);
        glVertex2f(173.9f, 250.9f);
    glEnd();

    // Polygon 5
    glBegin(GL_POLYGON);
        glVertex2f(193.8f, 277.5f);
        glVertex2f(200.0f, 260.0f);
        glVertex2f(262.0f, 285.9f);
        glVertex2f(281.5f, 279.1f);
        glVertex2f(277.3f, 287.5f);
        glVertex2f(272.4f, 297.4f);
        glVertex2f(267.8f, 306.5f);
    glEnd();

    // Polygon 6
    glBegin(GL_POLYGON);
        glVertex2f(272.4f, 297.4f);
        glVertex2f(267.8f, 306.5f);
        glVertex2f(500.0f, 400.0f);
        glVertex2f(505.4f, 389.1f);
    glEnd();

    // Polygon 7
    glBegin(GL_POLYGON);
        glVertex2f(281.5f, 279.1f);
        glVertex2f(277.3f, 287.5f);
        glVertex2f(510.9f, 377.7f);
        glVertex2f(516.8f, 367.3f);
    glEnd();

    // Polygon 8
    glBegin(GL_POLYGON);
        glVertex2f(500.0f, 400.0f);
        glVertex2f(505.4f, 389.1f);
        glVertex2f(602.6f, 424.9f);
        glVertex2f(709.9f, 464.3f);
        glVertex2f(730.0f, 471.7f);
        glVertex2f(726.0f, 480.7f);
    glEnd();

    // Polygon 9
    glBegin(GL_POLYGON);
        glVertex2f(505.4f, 389.1f);
        glVertex2f(510.9f, 377.7f);
        glVertex2f(516.8f, 367.3f);
        glVertex2f(560.1f, 379.6f);
        glVertex2f(581.1f, 385.5f);
        glVertex2f(605.4f, 392.4f);
        glVertex2f(602.6f, 424.9f);
    glEnd();

    // Polygon 10
    glBegin(GL_POLYGON);
        glVertex2f(604.0f, 407.2f);
        glVertex2f(605.4f, 392.4f);
        glVertex2f(699.1f, 430.9f);
        glVertex2f(704.1f, 446.4f);
    glEnd();

    // Polygon 11
    glBegin(GL_POLYGON);
        glVertex2f(717.0f, 439.0f);
        glVertex2f(699.1f, 430.9f);
        glVertex2f(704.1f, 446.4f);
        glVertex2f(709.9f, 464.3f);
        glVertex2f(730.0f, 471.7f);
    glEnd();

    // Polygon 12
    glBegin(GL_POLYGON);
        glVertex2f(560.1f, 379.6f);
        glVertex2f(581.1f, 385.5f);
        glVertex2f(581.2f, 352.6f);
        glVertex2f(581.5f, 270.7f);
        glVertex2f(581.6f, 206.9f);
        glVertex2f(580.0f, 151.9f);
        glVertex2f(560.0f, 151.9f);
        glVertex2f(559.9f, 206.9f);
    glEnd();

    // Polygon 13
    glBegin(GL_POLYGON);
        glVertex2f(581.6f, 206.9f);
        glVertex2f(580.0f, 151.9f);
        glVertex2f(634.3f, 151.9f);

    glEnd();
}

void drawLeftWall() {
    // Brick sizing
    const float brick_h = 15.0f;   // row height
    const float brick_w = 28.0f;   // nominal brick length

    // Polygon vertices for left wall
    float wallX[] = {0.0f, 182.1f, 173.1f, 174.2f, 0.0f};
    float wallY[] = {151.9f, 151.9f, 180.8f, 219.8f, 220.0f};
    int n = 5;

    // Draw main wall (brick fill)
    glColor3f(0.776f, 0.373f, 0.161f); // brick color
    glBegin(GL_POLYGON);
    for (int i = 0; i < n; i++) {
        glVertex2f(wallX[i], wallY[i]);
    }
    glEnd();

    // Mortar color
    glColor3f(0.08f, 0.08f, 0.08f);  // dark mortar
    glLineWidth(2.0f);

    // Horizontal mortar lines (from minY to maxY of polygon)
    float minY = 151.9f, maxY = 220.0f;
    for (float y = minY + brick_h; y < maxY; y += brick_h) {
        glBegin(GL_LINES);
            glVertex2f(0.0f, y);
            glVertex2f(174.2f, y); // span within polygon’s approximate width
        glEnd();
    }

    // Vertical mortar lines (approximate bounding box style, still staggered)
    int rows = (int)((maxY - minY) / brick_h + 0.999f);
    for (int r = 0; r < rows; ++r) {
        float row_bottom = minY + r * brick_h;
        float row_top = row_bottom + brick_h;
        if (row_top > maxY) row_top = maxY;

        float offset = (r % 2 == 0) ? 0.0f : (brick_w * 0.5f);

        for (float x = 0.0f + offset; x < 174.2f; x += brick_w) {
            if (x <= 1.0f || x >= 174.2f - 1.0f) continue;

            glBegin(GL_LINES);
                glVertex2f(x, row_bottom + 1.0f);
                glVertex2f(x, row_top - 1.0f);
            glEnd();
        }
    }

    // Restore line width
    glLineWidth(1.0f);
}

void drawLeftWindow() {
    // Window fill (glass-like)
    glColor3f(0.7f, 0.75f, 0.8f);
    glBegin(GL_POLYGON);
        glVertex2f(193.4f, 230.1f);  // S
        glVertex2f(277.4f, 230.2f);  // F5
        glVertex2f(277.1f, 182.0f);  // E5
        glVertex2f(192.7f, 180.8f);  // N
    glEnd();

    // Window border (dark frame)
    glColor3f(0.2f, 0.2f, 0.2f);
    glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(193.4f, 230.1f);
        glVertex2f(277.4f, 230.2f);
        glVertex2f(277.1f, 182.0f);
        glVertex2f(192.7f, 180.8f);
    glEnd();

    // --- Window grill (metal bars) ---
    glColor3f(0.1f, 0.1f, 0.1f);  // steel color
    glLineWidth(2.0f);

    // Vertical bars (3 sections)
    float leftX  = 193.4f;
    float rightX = 277.4f;
    float stepX = (rightX - leftX) / 3.0f;

    for (int i = 1; i < 3; i++) {
        float x = leftX + i * stepX;
        glBegin(GL_LINES);
            glVertex2f(x, 230.1f); // top
            glVertex2f(x, 182.0f); // bottom
        glEnd();
    }

    // Horizontal bars (2 sections)
    float topY    = 230.1f;
    float bottomY = 182.0f;
    float stepY = (topY - bottomY) / 2.0f;

    for (int j = 1; j < 2; j++) {
        float y = bottomY + j * stepY;
        glBegin(GL_LINES);
            glVertex2f(leftX, y);
            glVertex2f(rightX, y);
        glEnd();
    }

    glLineWidth(1.0f); // reset
}

void drawLeftWall1() {
    // Enable stencil test
    glEnable(GL_STENCIL_TEST);

    // Step 1: Write wall shape into stencil buffer
    glClear(GL_STENCIL_BUFFER_BIT);
    glStencilFunc(GL_ALWAYS, 1, 1);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glBegin(GL_POLYGON);
        glVertex2f(193.4f, 230.1f);  // S
        glVertex2f(200.0f, 260.0f);  // P
        glVertex2f(262.0f, 285.9f);  // W
        glVertex2f(281.5f, 279.1f);  // Z
        glVertex2f(277.4f, 230.2f);  // F5
    glEnd();
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    // Step 2: Wall fill
    glStencilFunc(GL_EQUAL, 1, 1);
    glColor3f(0.776f, 0.373f, 0.161f);  // brick color
    glBegin(GL_POLYGON);
        glVertex2f(193.4f, 230.1f);  // S
        glVertex2f(200.0f, 260.0f);  // P
        glVertex2f(262.0f, 285.9f);  // W
        glVertex2f(281.5f, 279.1f);  // Z
        glVertex2f(277.4f, 230.2f);  // F5
    glEnd();

    // Step 3: Brick pattern
    glColor3f(0.1f, 0.1f, 0.1f);  // mortar
    glLineWidth(2.0);

    const float brick_h = 12.0f;
    const float brick_w = 26.0f;

    float minX = 193.4f, maxX = 281.5f;
    float minY = 230.1f, maxY = 285.9f;

    int rows = (int)((maxY - minY) / brick_h + 0.5f);

    // Horizontal lines
    for (float y = minY + brick_h; y < maxY; y += brick_h) {
        glBegin(GL_LINES);
            glVertex2f(minX, y);
            glVertex2f(maxX, y);
        glEnd();
    }

    // Vertical (per row, staggered)
    for (int r = 0; r < rows; ++r) {
        float row_bottom = minY + r * brick_h;
        float row_top = row_bottom + brick_h;
        float offset = (r % 2 == 0) ? 0.0f : brick_w * 0.5f;

        for (float x = minX + offset; x < maxX; x += brick_w) {
            if (x <= minX + 1 || x >= maxX - 1) continue;
            glBegin(GL_LINES);
                glVertex2f(x, row_bottom + 1);
                glVertex2f(x, row_top - 1);
            glEnd();
        }
    }

    glDisable(GL_STENCIL_TEST);
    glLineWidth(1.0);
}

void drawLeftWall2() {
    // Wall fill
    glColor3f(0.776, 0.373, 0.161);
    glBegin(GL_POLYGON);
        glVertex2f(192.7f, 180.8f);   // N
        glVertex2f(277.1f, 182.0f);   // E5
        glVertex2f(277.1f, 151.9f);   // C5
        glVertex2f(204.1f, 151.9f);   // O
    glEnd();

    // Border
    glColor3f(0.2f, 0.2f, 0.2f);
    glLineWidth(2.0);
    glBegin(GL_LINE_LOOP);
        glVertex2f(192.7f, 180.8f);   // N
        glVertex2f(277.1f, 182.0f);   // E5
        glVertex2f(277.1f, 151.9f);   // C5
        glVertex2f(204.1f, 151.9f);   // O
    glEnd();

    // Brick pattern
    glColor3f(0.1f, 0.1f, 0.1f);
    glLineWidth(1.5);

    float minX = 204.0f, maxX = 277.0f;
    float minY = 151.9f, maxY = 180.8f;
    float brick_h = 7.0f;
    float brick_w = 18.0f;

    int rows = (int)((maxY - minY) / brick_h + 0.5f);

    // Horizontal lines
    for (float y = minY + brick_h; y < maxY; y += brick_h) {
        glBegin(GL_LINES);
            glVertex2f(minX, y);
            glVertex2f(maxX, y);
        glEnd();
    }

    // Vertical staggered
    for (int r = 0; r < rows; ++r) {
        float row_bottom = minY + r * brick_h;
        float row_top = row_bottom + brick_h;
        float offset = (r % 2 == 0) ? 0.0f : brick_w * 0.5f;

        for (float x = minX + offset; x < maxX; x += brick_w) {
            if (x <= minX + 1 || x >= maxX - 1) continue;
            glBegin(GL_LINES);
                glVertex2f(x, row_bottom + 1);
                glVertex2f(x, row_top - 1);
            glEnd();
        }
    }

    glLineWidth(1.0);
}

void drawLeftPillar1() {
    glColor3f(0.82f, 0.82f, 0.82f); // cement/light-gray color
    glBegin(GL_POLYGON);
        glVertex2f(328.6f, 269.3f);  // J3
        glVertex2f(328.6f, 151.9f);  // G5
        glVertex2f(346.1f, 151.9f);  // F
        glVertex2f(346.1f, 253.9f);  // G3
        glVertex2f(346.1f, 269.3f);  // I
    glEnd();

    // Border outline for sharpness
    glColor3f(0.25f, 0.25f, 0.25f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(328.6f, 269.3f);
        glVertex2f(328.6f, 151.9f);
        glVertex2f(346.1f, 151.9f);
        glVertex2f(346.1f, 253.9f);
        glVertex2f(346.1f, 269.3f);
    glEnd();
    glLineWidth(1.0f);
}

void drawLeftPillar2() {
    glColor3f(0.82f, 0.82f, 0.82f); // cement/light-gray color
    glBegin(GL_POLYGON);
        glVertex2f(346.1f, 151.0f);  // F
        glVertex2f(346.1f, 253.9f);  // G3
        glVertex2f(369.3f, 253.1f);  // H
        glVertex2f(369.3f, 241.2f);  // E
        glVertex2f(369.3f, 156.3f);  // W
    glEnd();

    // Border outline for sharpness
    glColor3f(0.25f, 0.25f, 0.25f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(346.1f, 151.0f);
        glVertex2f(346.1f, 253.9f);
        glVertex2f(369.3f, 253.1f);
        glVertex2f(369.3f, 241.2f);
        glVertex2f(369.3f, 156.3f);
    glEnd();
    glLineWidth(1.0f);
}

void drawCenterPillar() {
    glColor3f(0.82f, 0.82f, 0.82f); // cement/light-gray color
    glBegin(GL_POLYGON);
        glVertex2f(581.2f, 352.6f);  // J3
        glVertex2f(675.6f, 352.6f);  // G5
        glVertex2f(675.6f, 366.6f);  // F
        glVertex2f(581.2f, 366.6f);  // G3
        glVertex2f(581.2f, 352.6f);  // I
    glEnd();

}

void drawCenterWall1() {
    // Wall bounds
    const float x0 = 581.2f;   // Z
    const float x1 = 675.6f;   // V1
    const float y0 = 272.1f;   // U1
    const float y1 = 352.6f;   // Z/V1

    const float brick_h = 15.0f;
    const float brick_w = 28.0f;

    // Draw main wall
    glColor3f(0.776f, 0.373f, 0.161f);
    glBegin(GL_POLYGON);
        glVertex2f(581.2f, 352.6f);  // Z
        glVertex2f(675.6f, 352.6f);  // V1
        glVertex2f(676.5f, 272.1f);  // W1
        glVertex2f(581.5f, 272.1f);  // U1
    glEnd();

    // Mortar lines
    glColor3f(0.08f, 0.08f, 0.08f);
    glLineWidth(2.0f);

    for (float y = y0 + brick_h; y < y1; y += brick_h) {
        glBegin(GL_LINES);
            glVertex2f(x0, y);
            glVertex2f(x1, y);
        glEnd();
    }

    int rows = (int)((y1 - y0) / brick_h + 0.999f);
    for (int r = 0; r < rows; ++r) {
        float row_bottom = y0 + r * brick_h;
        float row_top = row_bottom + brick_h;
        if (row_top > y1) row_top = y1;

        float offset = (r % 2 == 0) ? 0.0f : (brick_w * 0.5f);

        for (float x = x0 + offset; x < x1; x += brick_w) {
            if (x <= x0 + 1.0f || x >= x1 - 1.0f) continue;
            glBegin(GL_LINES);
                glVertex2f(x, row_bottom + 1.0f);
                glVertex2f(x, row_top - 1.0f);
            glEnd();
        }
    }

    glLineWidth(1.0f);
}

void drawCenterWindow() {
    // Window fill (glass-like color)
    glColor3f(0.7f, 0.75f, 0.8f);
    glBegin(GL_POLYGON);
        glVertex2f(677.4f, 272.1f);  // W1
        glVertex2f(581.5f, 272.1f);  // U1
        glVertex2f(581.6f, 206.9f);  // Q1
        glVertex2f(677.4f, 208.7f);  // A2
    glEnd();

    // Window border (dark frame)
    glColor3f(0.2f, 0.2f, 0.2f);
    glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(677.4f, 272.1f);
        glVertex2f(581.5f, 272.1f);
        glVertex2f(581.6f, 206.9f);
        glVertex2f(677.4f, 208.7f);
    glEnd();

    // --- Window grill (metal bars) ---
    glColor3f(0.1f, 0.1f, 0.1f);  // dark steel color
    glLineWidth(2.0f);

    // Vertical bars (3 sections)
    float leftX  = 581.5f;
    float rightX = 677.4f;
    float stepX = (rightX - leftX) / 3.0f;

    for (int i = 1; i < 3; i++) {
        float x = leftX + i * stepX;
        glBegin(GL_LINES);
            glVertex2f(x, 272.1f); // top
            glVertex2f(x, 208.7f); // bottom
        glEnd();
    }

    // Horizontal bars (2 sections)
    float topY    = 272.1f;
    float bottomY = 208.7f;
    float stepY = (topY - bottomY) / 2.0f;

    for (int j = 1; j < 2; j++) {
        float y = bottomY + j * stepY;
        glBegin(GL_LINES);
            glVertex2f(leftX, y);
            glVertex2f(rightX, y);
        glEnd();
    }

    glLineWidth(1.0f); // reset
}

void drawCenterWall2() {
    // Wall fill
    glColor3f(0.776, 0.373, 0.161);
    glBegin(GL_POLYGON);
        glVertex2f(581.6f, 206.9f);  // Q1
        glVertex2f(677.4f, 208.7f);  // A2
        glVertex2f(678.1f, 151.9f);  // E2
        glVertex2f(634.3f, 151.9f);  // D2
    glEnd();

    // Border
    glColor3f(0.2f, 0.2f, 0.2f);
    glLineWidth(2.0);
    glBegin(GL_LINE_LOOP);
        glVertex2f(581.6f, 206.9f);  // Q1
        glVertex2f(677.4f, 208.7f);  // A2
        glVertex2f(678.1f, 151.9f);  // E2
        glVertex2f(634.3f, 151.9f);  // D2
    glEnd();

    // Brick pattern (clipped to wall bounds)
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilMask(0xFF);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    glBegin(GL_POLYGON);
        glVertex2f(581.6f, 206.9f);
        glVertex2f(677.4f, 208.7f);
        glVertex2f(678.1f, 151.9f);
        glVertex2f(634.3f, 151.9f);
    glEnd();

    glStencilFunc(GL_EQUAL, 1, 0xFF);
    glStencilMask(0x00);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    glColor3f(0.1f, 0.1f, 0.1f);
    glLineWidth(1.5);

    float minX = 582.0f, maxX = 677.0f;
    float minY = 151.9f, maxY = 206.0f;
    float brick_h = 7.0f;
    float brick_w = 18.0f;

    int rows = (int)((maxY - minY) / brick_h + 0.5f);

    for (float y = minY + brick_h; y < maxY; y += brick_h) {
        glBegin(GL_LINES);
            glVertex2f(minX, y);
            glVertex2f(maxX, y);
        glEnd();
    }

    for (int r = 0; r < rows; ++r) {
        float row_bottom = minY + r * brick_h;
        float row_top = row_bottom + brick_h;
        float offset = (r % 2 == 0) ? 0.0f : brick_w * 0.5f;

        for (float x = minX + offset; x < maxX; x += brick_w) {
            if (x <= minX + 1 || x >= maxX - 1) continue;
            glBegin(GL_LINES);
                glVertex2f(x, row_bottom + 1);
                glVertex2f(x, row_top - 1);
            glEnd();
        }
    }

    glDisable(GL_STENCIL_TEST);
    glLineWidth(1.0);
}

void drawRightWall() {
    // Brick wall polygon using new points
    float x0 = 1070.9f, y0 = 231.2f;
    float x1 = 1200.0f, y1 = 231.2f;
    float x2 = 1200.0f, y2 = 151.9f;
    float x3 = 1062.5f, y3 = 151.9f;

    // Draw the main wall (brick fill)
    glColor3f(0.776f, 0.373f, 0.161f); // brick color
    glBegin(GL_POLYGON);
        glVertex2f(x0, y0);
        glVertex2f(x1, y1);
        glVertex2f(x2, y2);
        glVertex2f(x3, y3);
    glEnd();

    // Mortar color & thickness
    glColor3f(0.08f, 0.08f, 0.08f);
    glLineWidth(2.0f);

    // Horizontal mortar lines
    float minY = y3;
    float maxY = y1;
    const float brick_h = 15.0f;
    for (float y = minY + brick_h; y < maxY; y += brick_h) {
        glBegin(GL_LINES);
            glVertex2f(x3, y);
            glVertex2f(x1, y);
        glEnd();
    }

    // Vertical mortar lines per row (staggered pattern)
    const float brick_w = 28.0f;
    int rows = (int)((maxY - minY) / brick_h + 0.999f);

    for (int r = 0; r < rows; ++r) {
        float row_bottom = minY + r * brick_h;
        float row_top = row_bottom + brick_h;
        if (row_top > maxY) row_top = maxY;

        // Alternate offset every row (running bond)
        float offset = (r % 2 == 0) ? 0.0f : brick_w * 0.5f;

        // Draw vertical lines only for this row
        for (float x = x3 + offset; x < x1; x += brick_w) {
            if (x <= x3 + 1.0f || x >= x1 - 1.0f) continue;
            glBegin(GL_LINES);
                glVertex2f(x, row_bottom + 1.0f);
                glVertex2f(x, row_top - 1.0f);
            glEnd();
        }
    }

    glLineWidth(1.0f); // Restore line width
}

void drawRightWall1() {
    // Polygon points
    float x0 = 987.1f, y0 = 293.6f;
    float x1 = 986.9f, y1 = 237.2f;
    float x2 = 1052.5f, y2 = 237.2f;
    float x3 = 1052.1f, y3 = 268.1f;

    // Draw main wall polygon
    glColor3f(0.776f, 0.373f, 0.161f); // brick color
    glBegin(GL_POLYGON);
        glVertex2f(x0, y0);
        glVertex2f(x1, y1);
        glVertex2f(x2, y2);
        glVertex2f(x3, y3);
    glEnd();

    // Mortar color & thickness
    glColor3f(0.08f, 0.08f, 0.08f);
    glLineWidth(2.0f);

    // Horizontal mortar lines
    float minY = y1;
    float maxY = y0;
    const float brick_h = 12.0f; // adjusted for this wall
    for (float y = minY + brick_h; y < maxY; y += brick_h) {
        glBegin(GL_LINES);
            glVertex2f(x1, y);
            glVertex2f(x2, y);
        glEnd();
    }

    // Vertical mortar lines per row (staggered pattern)
    const float brick_w = 26.0f;
    int rows = (int)((maxY - minY) / brick_h + 0.999f);

    for (int r = 0; r < rows; ++r) {
        float row_bottom = minY + r * brick_h;
        float row_top = row_bottom + brick_h;
        if (row_top > maxY) row_top = maxY;

        float offset = (r % 2 == 0) ? 0.0f : brick_w * 0.5f;

        for (float x = x1 + offset; x < x2; x += brick_w) {
            if (x <= x1 + 1.0f || x >= x2 - 1.0f) continue;
            glBegin(GL_LINES);
                glVertex2f(x, row_bottom + 1.0f);
                glVertex2f(x, row_top - 1.0f);
            glEnd();
        }
    }

    glLineWidth(1.0f); // restore
}

void drawRightWall2() {
    // Brick wall polygon points
    float x0 = 987.4f, y0 = 151.9f;
    float x1 = 1038.0f, y1 = 151.9f;
    float x2 = 1053.0f, y2 = 187.8f;
    float x3 = 987.4f, y3 = 187.8f;

    // Draw the main wall (brick fill)
    glColor3f(0.776f, 0.373f, 0.161f); // brick color
    glBegin(GL_POLYGON);
        glVertex2f(x0, y0);
        glVertex2f(x1, y1);
        glVertex2f(x2, y2);
        glVertex2f(x3, y3);
    glEnd();

    // Mortar lines
    glColor3f(0.08f, 0.08f, 0.08f); // dark gray
    glLineWidth(2.0f);

    // Calculate min/max
    float minX = x0 < x1 ? x0 : x1;
    minX = minX < x3 ? minX : x3;
    float maxX = x1 > x2 ? x1 : x2;
    maxX = maxX > x3 ? maxX : x3;

    float minY = y0 < y1 ? y0 : y1;
    minY = minY < y3 ? minY : y3;
    float maxY = y2 > y3 ? y2 : y3;
    maxY = maxY > y1 ? maxY : y1;

    const float brick_h = 15.0f;
    const float brick_w = 28.0f;

    // Horizontal lines
    for (float y = minY + brick_h; y < maxY; y += brick_h) {
        glBegin(GL_LINES);
            glVertex2f(minX, y);
            glVertex2f(maxX, y);
        glEnd();
    }

    // Vertical lines with staggered pattern
    int rows = (int)((maxY - minY) / brick_h + 0.999f);
    for (int r = 0; r < rows; ++r) {
        float row_bottom = minY + r * brick_h;
        float row_top = row_bottom + brick_h;
        if (row_top > maxY) row_top = maxY;

        float offset = (r % 2 == 0) ? 0.0f : brick_w / 2.0f;
        for (float x = minX + offset; x < maxX; x += brick_w) {
            if (x <= minX + 1.0f || x >= maxX - 1.0f) continue;
            glBegin(GL_LINES);
                glVertex2f(x, row_bottom + 1.0f);
                glVertex2f(x, row_top - 1.0f);
            glEnd();
        }
    }

    glLineWidth(1.0f); // reset
}

void drawRightWindow() {
    // Window fill (glass-like color)
    glColor3f(0.7f, 0.75f, 0.8f);
    glBegin(GL_POLYGON);
        glVertex2f(1053.0f, 237.2f);   // W1
        glVertex2f(986.4f, 237.2f);    // U1
        glVertex2f(986.4f, 187.9f);    // Q1
        glVertex2f(1053.0f, 187.5f);   // A2
    glEnd();

    // Window border (dark frame)
    glColor3f(0.2f, 0.2f, 0.2f);
    glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(1053.0f, 238.2f);
        glVertex2f(986.9f, 237.2f);
        glVertex2f(986.4f, 187.9f);
        glVertex2f(1053.0f, 187.5f);
    glEnd();

    // --- Window grill (metal bars) ---
    glColor3f(0.1f, 0.1f, 0.1f);  // dark steel color
    glLineWidth(2.0f);

    // Vertical bars (3 sections)
    float leftX  = 986.9f;
    float rightX = 1053.0f;
    float stepX = (rightX - leftX) / 3.0f;

    for (int i = 1; i < 3; i++) {
        float x = leftX + i * stepX;
        glBegin(GL_LINES);
            glVertex2f(x, 238.2f); // top
            glVertex2f(x, 187.5f); // bottom
        glEnd();
    }

    // Horizontal bars (2 sections)
    float topY    = 238.2f;
    float bottomY = 187.5f;
    float stepY = (topY - bottomY) / 2.0f;

    for (int j = 1; j < 2; j++) {
        float y = bottomY + j * stepY;
        glBegin(GL_LINES);
            glVertex2f(leftX, y);
            glVertex2f(rightX, y);
        glEnd();
    }

    glLineWidth(1.0f); // reset
}

void drawEntryGate1() {
    // ---------- Gate frame (black-green shade) ----------
    glColor3f(0.05f, 0.15f, 0.05f); // dark greenish-black
    glBegin(GL_POLYGON);
        glVertex2f(370.3f, 240.3f); // top-left
        glVertex2f(462.3f, 240.2f); // top-right
        glVertex2f(462.3f, 151.9f); // bottom-right
        glVertex2f(370.1f, 151.0f); // bottom-left
    glEnd();

    // Frame border
    glColor3f(0.0f, 0.2f, 0.1f); // slightly lighter dark-green for border
    glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(370.3f, 240.3f);
        glVertex2f(462.3f, 240.2f);
        glVertex2f(462.3f, 151.9f);
        glVertex2f(370.1f, 151.0f);
    glEnd();

    // ---------- Vertical metal grill ----------
    glColor3f(0.6f, 0.6f, 0.6f); // metallic gray
    glLineWidth(2.0f);

    int numVerticalBars = 8; // number of grill bars
    float xStart = 370.3f;
    float xEnd = 462.3f;
    float yBottom = 151.0f;
    float yTop = 240.3f;

    for (int i = 1; i < numVerticalBars; i++) {
        float x = xStart + i * (xEnd - xStart) / numVerticalBars;
        glBegin(GL_LINES);
            glVertex2f(x, yBottom); // bottom
            glVertex2f(x, yTop);    // top
        glEnd();
    }

    glLineWidth(1.0f);
}

void drawEntryGate2() {
    // ---------- Gate frame (black-green shade) ----------
    glColor3f(0.05f, 0.15f, 0.05f); // dark greenish-black
    glBegin(GL_POLYGON);
        glVertex2f(977.1f, 240.2f); // top-left
        glVertex2f(915.8f, 240.2f); // top-right
        glVertex2f(915.8f, 151.9f); // bottom-right
        glVertex2f(977.1f, 151.9f); // bottom-left
    glEnd();

    // Frame border
    glColor3f(0.0f, 0.2f, 0.1f); // slightly lighter dark-green for border
    glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(977.1f, 240.2f);
        glVertex2f(915.8f, 240.2f);
        glVertex2f(915.8f, 151.9f);
        glVertex2f(977.1f, 151.9f);
    glEnd();

    // ---------- Vertical metal grill ----------
    glColor3f(0.6f, 0.6f, 0.6f); // metallic gray
    glLineWidth(2.0f);

    int numVerticalBars = 8; // number of grill bars
    float xStart = 977.1f;
    float xEnd = 915.8f;
    float yBottom = 151.9f;
    float yTop = 240.2f;

    for (int i = 1; i < numVerticalBars; i++) {
        float x = xStart + i * (xEnd - xStart) / numVerticalBars;
        glBegin(GL_LINES);
            glVertex2f(x, yBottom); // bottom
            glVertex2f(x, yTop);    // top
        glEnd();
    }

    glLineWidth(1.0f);
}

void drawDualGate1() {
    // ---------- Gate frame (black-green shade) ----------
    glColor3f(0.05f, 0.15f, 0.05f); // dark greenish-black
    glBegin(GL_POLYGON);
        glVertex2f(695.9f, 247.3f); // top-left
        glVertex2f(695.9f, 151.9f); // bottom-left
        glVertex2f(787.8f, 151.9f); // bottom-right
        glVertex2f(787.8f, 247.3f); // top-right
    glEnd();

    // Frame border
    glColor3f(0.0f, 0.2f, 0.1f); // slightly lighter dark-green for border
    glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(695.9f, 247.3f);
        glVertex2f(695.9f, 151.9f);
        glVertex2f(787.8f, 151.9f);
        glVertex2f(787.8f, 247.3f);
    glEnd();

    // ---------- Vertical metal grill ----------
    glColor3f(0.6f, 0.6f, 0.6f); // metallic gray
    glLineWidth(2.0f);

    int numVerticalBars = 8; // number of grill bars
    float xStart = 695.9f;
    float xEnd = 787.8f;
    float yBottom = 151.9f;
    float yTop = 247.3f;

    for (int i = 1; i < numVerticalBars; i++) {
        float x = xStart + i * (xEnd - xStart) / numVerticalBars;
        glBegin(GL_LINES);
            glVertex2f(x, yBottom); // bottom
            glVertex2f(x, yTop);    // top
        glEnd();
    }

    glLineWidth(1.0f);
}

void drawDualGate2() {
    // ---------- Gate frame (black-green shade) ----------
    glColor3f(0.05f, 0.15f, 0.05f); // dark greenish-black
    glBegin(GL_POLYGON);
        glVertex2f(796.9f, 247.3f); // top-left
        glVertex2f(796.9f, 151.9f); // bottom-left
        glVertex2f(891.6f, 151.9f); // bottom-right
        glVertex2f(891.6f, 247.3f); // top-right
    glEnd();

    // Frame border
    glColor3f(0.0f, 0.2f, 0.1f); // slightly lighter dark-green for border
    glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(796.9f, 247.3f);
        glVertex2f(796.9f, 151.9f);
        glVertex2f(891.6f, 151.9f);
        glVertex2f(891.6f, 247.3f);
    glEnd();

    // ---------- Vertical metal grill ----------
    glColor3f(0.6f, 0.6f, 0.6f); // metallic gray
    glLineWidth(2.0f);

    int numVerticalBars = 8; // number of grill bars
    float xStart = 796.9f;
    float xEnd = 891.6f;
    float yBottom = 151.9f;
    float yTop = 247.3f;

    for (int i = 1; i < numVerticalBars; i++) {
        float x = xStart + i * (xEnd - xStart) / numVerticalBars;
        glBegin(GL_LINES);
            glVertex2f(x, yBottom); // bottom
            glVertex2f(x, yTop);    // top
        glEnd();
    }

    glLineWidth(1.0f);
}

void drawRightPillar() {
    glColor3f(0.82f, 0.82f, 0.82f);

    // Polygon 1
    glBegin(GL_POLYGON);
        glVertex2f(1038.1f, 151.9f);
        glVertex2f(1062.5f, 151.9f);
        glVertex2f(1071.6f, 188.3f);
        glVertex2f(1053.0f, 187.5f);
    glEnd();

    // Polygon 2
    glBegin(GL_POLYGON);
        glVertex2f(1071.6f, 188.3f);
        glVertex2f(1053.0f, 187.5f);
        glVertex2f(1052.5f, 238.2f);
        glVertex2f(1052.1f, 268.1f);
        glVertex2f(1070.4f, 260.9f);

    glEnd();

    // Polygon 3
    glBegin(GL_POLYGON);
        glVertex2f(1091.3f, 252.7f);
        glVertex2f(1098.6f, 264.4f);
        glVertex2f(920.8f, 332.9f);
        glVertex2f(916.8f, 321.2f);
        glVertex2f(970.7f, 300.0f);
        glVertex2f(982.1f, 295.6f);
        glVertex2f(987.1f, 293.6f);
        glVertex2f(1052.1f, 268.1f);
        glVertex2f(1070.4f, 260.9f);
    glEnd();

    // Polygon 4
    glBegin(GL_POLYGON);
        glVertex2f(920.8f, 332.9f);
        glVertex2f(916.8f, 321.2f);
        glVertex2f(902.7f, 316.3f);
        glVertex2f(889.4f, 311.6f);
        glVertex2f(892.1f, 321.7f);
    glEnd();

    // Polygon 5
    glBegin(GL_POLYGON);
        glVertex2f(889.4f, 311.6f);
        glVertex2f(902.7f, 316.3f);
        glVertex2f(959.6f, 296.9f);
        glVertex2f(959.3f, 288.4f);
    glEnd();

    // Polygon 6
    glBegin(GL_POLYGON);
        glVertex2f(959.6f, 296.9f);
        glVertex2f(959.3f, 288.4f);
        glVertex2f(982.1f, 295.6f);
        glVertex2f(970.7f, 300.0f);
    glEnd();
}

void drawRightPillar1() {
    glColor3f(0.82f, 0.82f, 0.82f); // cement/light-gray color
    glBegin(GL_POLYGON);
        glVertex2f(917.8f, 151.9f);
        glVertex2f(915.8f, 246.8f);
        glVertex2f(915.8f, 274.3f);
        glVertex2f(895.3f, 274.3f);
        glVertex2f(895.3f, 151.9f);
    glEnd();

    // Border outline for sharpness
    glColor3f(0.25f, 0.25f, 0.25f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(917.8f, 151.9f);
        glVertex2f(915.8f, 246.8f);
        glVertex2f(915.8f, 274.3f);
        glVertex2f(895.3f, 274.3f);
        glVertex2f(895.3f, 151.9f);
    glEnd();
    glLineWidth(1.0f);
}

void drawLock1() {
    glColor3f(0.21f, 0.27f, 0.31f); // charcoal grey
    glLineWidth(2.0f);
    glBegin(GL_LINE_STRIP);
        glVertex2f(677.4f, 223.0f);
        glVertex2f(695.9f, 223.0f);
        glVertex2f(695.9f, 199.0f);
        glVertex2f(677.4f, 199.0f);
        glVertex2f(677.4f, 223.0f); // close the rectangle
    glEnd();
    glLineWidth(1.0f); // reset
}

void drawLock2() {
    glColor3f(0.21f, 0.27f, 0.31f); // charcoal grey
    glLineWidth(2.0f);
    glBegin(GL_LINE_STRIP);
        glVertex2f(787.8f, 223.0f);
        glVertex2f(796.9f, 223.0f);
        glVertex2f(796.9f, 199.0f);
        glVertex2f(787.8f, 199.0f);
        glVertex2f(787.8f, 223.0f); // close the rectangle
    glEnd();
    glLineWidth(1.0f); // reset
}

void drawLock3() {
    glColor3f(0.21f, 0.27f, 0.31f); // charcoal grey
    glLineWidth(2.0f);
    glBegin(GL_LINE_STRIP);
        glVertex2f(977.1f, 223.0f);
        glVertex2f(986.4f, 223.0f);
        glVertex2f(986.4f, 199.0f);
        glVertex2f(977.1f, 199.0f);
        glVertex2f(977.1f, 223.0f); // close the rectangle
    glEnd();
    glLineWidth(1.0f); // reset
}

void drawSecurityGuard() {
    float xMin = 495.3f, xMax = 515.3f;
    float yMin = 152.1f, yMax = 289.0f;
    float centerX = (xMin + xMax) / 2.0f;

    float scale = 2.0f; // guard size

    // --- Body (torso) ---
    glColor3f(0.0f, 0.0f, 0.4f); // navy blue
    glBegin(GL_POLYGON);
        glVertex2f(centerX - 6 * scale, yMin + 12);
        glVertex2f(centerX + 6 * scale, yMin + 12);
        glVertex2f(centerX + 6 * scale, yMin + 34);
        glVertex2f(centerX - 6 * scale, yMin + 34);
    glEnd();

    // --- Head (smaller, natural) ---
    glColor3f(1.0f, 0.87f, 0.68f); // skin tone
    float headRadius = 4 * scale;
    float headCenterY = yMin + 42;
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(centerX, headCenterY);
        for (int i = 0; i <= 40; i++) {
            float angle = i * 2.0f * M_PI / 40;
            glVertex2f(centerX + cos(angle) * headRadius,
                       headCenterY + sin(angle) * headRadius);
        }
    glEnd();

    // --- Facial features (subtle) ---
    glColor3f(0.0f, 0.0f, 0.0f); // black eyes
    glPointSize(2.0f);
    glBegin(GL_POINTS);
        glVertex2f(centerX - 2 * scale, headCenterY + 1 * scale); // left eye
        glVertex2f(centerX + 2 * scale, headCenterY + 1 * scale); // right eye
    glEnd();

    // Mouth (short line, neutral)
    glBegin(GL_LINES);
        glVertex2f(centerX - 2 * scale, headCenterY - 2 * scale);
        glVertex2f(centerX + 2 * scale, headCenterY - 2 * scale);
    glEnd();

    // --- Cap (flat instead of pointed) ---
    glColor3f(0.1f, 0.1f, 0.1f);
    glBegin(GL_QUADS);
        glVertex2f(centerX - 6 * scale, headCenterY + 2);
        glVertex2f(centerX + 6 * scale, headCenterY + 2);
        glVertex2f(centerX + 6 * scale, headCenterY + 5);
        glVertex2f(centerX - 6 * scale, headCenterY + 5);
    glEnd();

    // --- Left Arm (static) ---
    glColor3f(0.0f, 0.0f, 0.4f);
    glBegin(GL_QUADS);
        glVertex2f(centerX - 6 * scale, yMin + 32);
        glVertex2f(centerX - 8 * scale, yMin + 32);
        glVertex2f(centerX - 8 * scale, yMin + 16);
        glVertex2f(centerX - 6 * scale, yMin + 16);
    glEnd();

    // --- Right Arm (animated salute) ---
    glPushMatrix();
        glTranslatef(centerX + 6 * scale, yMin + 32, 0);
        glRotatef(-saluteAngle, 0, 0, 1);
        glColor3f(0.0f, 0.0f, 0.4f);
        glBegin(GL_QUADS);
            glVertex2f(0, 0);
            glVertex2f(2 * scale, 0);
            glVertex2f(2 * scale, 12 * scale);
            glVertex2f(0, 12 * scale);
        glEnd();

        // Hand
        glColor3f(1.0f, 0.87f, 0.68f);
        glBegin(GL_QUADS);
            glVertex2f(0, 12 * scale);
            glVertex2f(4 * scale, 12 * scale);
            glVertex2f(4 * scale, 14 * scale);
            glVertex2f(0, 14 * scale);
        glEnd();
    glPopMatrix();

    // --- Legs ---
    glColor3f(0.0f, 0.0f, 0.3f);
    glBegin(GL_QUADS);
        glVertex2f(centerX - 3 * scale, yMin + 12);
        glVertex2f(centerX - 5 * scale, yMin);
        glVertex2f(centerX - 2 * scale, yMin);
        glVertex2f(centerX, yMin + 12);

        glVertex2f(centerX + 3 * scale, yMin + 12);
        glVertex2f(centerX + 2 * scale, yMin);
        glVertex2f(centerX + 5 * scale, yMin);
        glVertex2f(centerX, yMin + 12);
    glEnd();

    // --- Badge ---
    glColor3f(1.0f, 0.84f, 0.0f);
    glBegin(GL_QUADS);
        glVertex2f(centerX - 2 * scale, yMin + 20);
        glVertex2f(centerX, yMin + 20);
        glVertex2f(centerX, yMin + 24);
        glVertex2f(centerX - 2 * scale, yMin + 24);
    glEnd();
}

void drawSecurityRoom() {
    // ---------- Security room polygon ----------
    glColor3f(0.7f, 0.7f, 0.7f); // light gray room
    glBegin(GL_POLYGON);
        glVertex2f(277.4f, 230.2f); // top-left
        glVertex2f(328.6f, 230.2f); // top-right
        glVertex2f(328.6f, 151.9f); // bottom-right
        glVertex2f(277.4f, 151.9f); // bottom-left
    glEnd();

    // ---------- Border outline ----------
    glColor3f(0.1f, 0.1f, 0.1f); // black border
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(277.4f, 230.2f);
        glVertex2f(328.6f, 230.2f);
        glVertex2f(328.6f, 151.9f);
        glVertex2f(277.4f, 151.9f);
    glEnd();
    glLineWidth(1.0f);

    // ---------- Door (shifted to x = 283, width 20, height 200) ----------
    glColor3f(0.3f, 0.15f, 0.0f); // darker brown door
    glBegin(GL_POLYGON);
        glVertex2f(283.5f, 151.9f);  // bottom-left
        glVertex2f(300.5f, 151.9f);  // bottom-right
        glVertex2f(300.5f, 199.0f);  // top-right
        glVertex2f(283.5f, 199.0f);  // top-left
    glEnd();

    glColor3f(0.0f, 0.0f, 0.0f); // door border
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(283.5f, 151.9f);
        glVertex2f(300.5f, 151.9f);
        glVertex2f(300.5f, 199.0f);
        glVertex2f(283.5f, 199.0f);
    glEnd();

    // ---------- Window (top-right) ----------
    glColor3f(0.7f, 0.85f, 0.9f); // glass blue
    glBegin(GL_POLYGON);
        glVertex2f(310.0f, 210.0f);  // bottom-left
        glVertex2f(325.0f, 210.0f);  // bottom-right
        glVertex2f(325.0f, 225.0f);  // top-right
        glVertex2f(310.0f, 225.0f);  // top-left
    glEnd();

    glColor3f(0.0f, 0.0f, 0.0f); // window border
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(310.0f, 210.0f);
        glVertex2f(325.0f, 210.0f);
        glVertex2f(325.0f, 225.0f);
        glVertex2f(310.0f, 225.0f);
    glEnd();
    glLineWidth(1.0f);
}

void applyNightOverlay() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(0.0f, 0.0f, 0.1f, nightOpacity); // dark overlay
    glBegin(GL_QUADS);
        glVertex2f(0, 550);
        glVertex2f(1200, 550);
        glVertex2f(1200, 800);
        glVertex2f(0, 800);
    glEnd();

    glDisable(GL_BLEND);
}

void update(int value) {
// ---- Car movement ----
for (int i = 0; i < NUM_CARS; i++) {
    cars[i].x += cars[i].speed;

    if (cars[i].x > 1200) {   // screen width = 1200
        cars[i].x = -160;     // reset off-screen to left
    }
}
    // ---- Cloud animation ----
    for (int i = 0; i < numClouds; i++) {
        clouds[i].x += clouds[i].speed;
        if (clouds[i].x - clouds[i].size*2 > 1200) { // off right edge
            clouds[i].x = -clouds[i].size*2;          // re-enter from left
        }
    }
    // ---- Security Guard salute animation ----
    saluteAngle += saluteDir * 2.0f; // change speed by tweaking 2.0f
    if (saluteAngle > 40.0f) saluteDir = -1; // arm down
    if (saluteAngle < 0.0f) saluteDir = 1;   // arm up

        // ---- Grass sway animation ----
    swayAngle += 0.05f;
    if (swayAngle > 2 * M_PI) swayAngle = 0;

    // ---- Sun horizontal animation ----
    sunX += sunSpeed;
    if (sunX >= SUN_MAX_X || sunX <= SUN_MIN_X) sunSpeed *= -1; // reverse at edges

    // ---- Night opacity calculation ----
    if (sunX <= SUN_MIN_X) nightOpacity = 0.0f;        // day
    else if (sunX >= SUN_MAX_X) nightOpacity = 1.0f;   // night
    else nightOpacity = (sunX - SUN_MIN_X) / (SUN_MAX_X - SUN_MIN_X);

    glutPostRedisplay();
    glutTimerFunc(30, update, 0); // keep unified timing
}

// DISPLAY FUNCTION
// ============================
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    drawGreenView();
    drawStreet();
    // Draw multiple cars
    for (int i = 0; i < NUM_CARS; i++) {
        drawCar(cars[i].x, cars[i].y);
    }
    drawSky();
    drawLeftWall();
    drawRightWall();
    drawRightWall1();
    drawRightWall2();
    drawCenterWall1();
    drawRightWindow();
    drawLeftPillars();
    drawCenterWindow();
    drawLeftWall1();
    drawLeftWindow();
    drawLeftWall2() ;
    drawCenterWall2();
    drawRightPillar();
    drawLeftPillar1();
    drawLeftPillar2();
    drawEntryGate1();
    drawEntryGate2();
    drawDualGate1();
    drawDualGate2();
    drawRightPillar1();
    drawLock1();
    drawLock2();
    drawLock3();
    drawSecurityGuard();
    drawGrass();
    drawCenterPillar();
    drawSecurityRoom();
    drawTree(40.0f, 450.0f);
    drawTree(160.0f, 400.0f);
    drawTree(80.0f, 320.0f);
    drawTree(270.0f, 470.0f);
    drawTree(350.0f, 380.0f);
    drawTree(440.0f, 450.0f);
    drawTree(595.0f, 460.0f);
    drawTree(790.0f, 400.0f);
    drawTree(900.0f, 450.0f);
    drawTree(1000.0f, 390.0f);
    drawTree(1100.0f, 450.0f);
    applyNightOverlay();

    glutSwapBuffers();
}
// RESIZE HANDLER
// ============================

void handleResize(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, 1200.0, 0.0, 800.0);
    glMatrixMode(GL_MODELVIEW);
}
// MAIN FUNCTION
// ============================
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1200, 800);
    glutCreateWindow("CUET Gate");
    glClearColor(1.0, 1.0, 1.0, 1.0);
    initGrass();
    glutDisplayFunc(display);

    glutReshapeFunc(handleResize);
    glutTimerFunc(30, update, 0);
    glutMainLoop();
    return 0;
}
