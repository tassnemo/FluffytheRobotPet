#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define STAT_MAX      10U
#define LOW_WARNING    3U
#define NAME_LEN      12U

#define BIT_ASLEEP     0U
#define BIT_HUNGRY     1U
#define BIT_SAD        2U
#define BIT_SICK       3U

#define SET_BIT(reg, n)    ((reg) |= (uint8_t)(1U << (n)))
#define CLR_BIT(reg, n)    ((reg) &= (uint8_t)~(1U << (n)))
#define READ_BIT(reg, n)   ((uint8_t)(((reg) >> (n)) & 1U))

typedef struct {
    char     name[NAME_LEN];
    uint8_t  food;
    uint8_t  fun;
    uint8_t  energy;
    uint8_t  mood;
    uint16_t hours;
} Pet_t;

static Pet_t fluffy;

static void hatchPet(void)
{
    strncpy(fluffy.name, "Fluffy", NAME_LEN - 1U);
    fluffy.name[NAME_LEN - 1U] = '\0';
    fluffy.food = STAT_MAX;
    fluffy.fun = STAT_MAX;
    fluffy.energy = STAT_MAX;
    fluffy.hours = 0U;
    CLR_BIT(fluffy.mood, BIT_ASLEEP);
    CLR_BIT(fluffy.mood, BIT_HUNGRY);
    CLR_BIT(fluffy.mood, BIT_SAD);
    CLR_BIT(fluffy.mood, BIT_SICK);
}

static void drawFace(void)
{
    printf("+------------------+\n");
    printf("| ");
    if (READ_BIT(fluffy.mood, BIT_ASLEEP)) {
        printf("- -   z z\n");
    } else if (READ_BIT(fluffy.mood, BIT_SICK)) {
        printf("o o   x x\n");
    } else if (READ_BIT(fluffy.mood, BIT_SAD)) {
        printf("o o   ~~~\n");
    } else {
        printf("o o   ^_^\n");
    }
    printf("|\n");
    printf("+------------------+\n");
}

static void drawStat(const char *label, uint8_t value)
{
    uint8_t i;

    printf("%s: ", label);
    for (i = 0U; i < STAT_MAX; ++i) {
        if (i < value) {
            printf("#");
        } else {
            printf("-");
        }
    }
    printf(" %u\n", value);
}

static void feed(void)
{
    if (READ_BIT(fluffy.mood, BIT_ASLEEP)) {
        puts("Wake Fluffy up first.");
        return;
    }
    if (fluffy.food >= STAT_MAX) {
        SET_BIT(fluffy.mood, BIT_SICK);
        puts("Fluffy is too full and feels sick.");
        return;
    }
    fluffy.food += 3U;
    if (fluffy.food > STAT_MAX) {
        fluffy.food = STAT_MAX;
    }
    puts("Fluffy happily eats.");
}

static void play(void)
{
    if (READ_BIT(fluffy.mood, BIT_ASLEEP)) {
        puts("Fluffy is asleep and cannot play.");
        return;
    }
    fluffy.fun += 3U;
    if (fluffy.fun > STAT_MAX) {
        fluffy.fun = STAT_MAX;
    }
    if (fluffy.energy >= 2U) {
        fluffy.energy -= 2U;
    } else {
        fluffy.energy = 0U;
    }
    puts("Fluffy plays and gets tired.");
}

static void sleepPet(void)
{
    if (READ_BIT(fluffy.mood, BIT_ASLEEP)) {
        CLR_BIT(fluffy.mood, BIT_ASLEEP);
        puts("Fluffy wakes up.");
    } else {
        SET_BIT(fluffy.mood, BIT_ASLEEP);
        puts("Fluffy goes to sleep.");
    }
}

static void updateMood(void)
{
    if (fluffy.food <= LOW_WARNING) {
        SET_BIT(fluffy.mood, BIT_HUNGRY);
    } else {
        CLR_BIT(fluffy.mood, BIT_HUNGRY);
    }

    if (fluffy.fun <= LOW_WARNING) {
        SET_BIT(fluffy.mood, BIT_SAD);
    } else {
        CLR_BIT(fluffy.mood, BIT_SAD);
    }

    if (READ_BIT(fluffy.mood, BIT_ASLEEP) || fluffy.energy == 0U) {
        SET_BIT(fluffy.mood, BIT_ASLEEP);
    } else {
        CLR_BIT(fluffy.mood, BIT_ASLEEP);
    }

    if (READ_BIT(fluffy.mood, BIT_SICK)) {
        if (READ_BIT(fluffy.mood, BIT_ASLEEP)) {
            CLR_BIT(fluffy.mood, BIT_SICK);
        }
    } else {
        CLR_BIT(fluffy.mood, BIT_SICK);
    }
}

static void hourPasses(void)
{
    if (fluffy.food > 0U) {
        fluffy.food -= 1U;
    }
    if (fluffy.fun > 0U) {
        fluffy.fun -= 1U;
    }
    if (READ_BIT(fluffy.mood, BIT_ASLEEP)) {
        fluffy.energy += 2U;
        if (fluffy.energy > STAT_MAX) {
            fluffy.energy = STAT_MAX;
        }
    } else if (fluffy.energy > 0U) {
        fluffy.energy -= 1U;
    }
    fluffy.hours += 1U;
    updateMood();
}

static uint8_t isHappy(void)
{
    if (fluffy.food <= LOW_WARNING || fluffy.fun <= LOW_WARNING ||
        fluffy.energy <= LOW_WARNING) {
        return 0U;
    }
    if (READ_BIT(fluffy.mood, BIT_HUNGRY) || READ_BIT(fluffy.mood, BIT_SAD) ||
        READ_BIT(fluffy.mood, BIT_ASLEEP) || READ_BIT(fluffy.mood, BIT_SICK)) {
        return 0U;
    }
    return 1U;
}

static void petReport(void)
{
    uint8_t bit;

    printf("\nPet name: %s\n", fluffy.name);
    printf("Hours alive: %u\n", fluffy.hours);
    drawStat("Food", fluffy.food);
    drawStat("Fun", fluffy.fun);
    drawStat("Energy", fluffy.energy);
    printf("Mood byte: ");
    for (bit = 0U; bit < 8U; ++bit) {
        printf("%u", (uint8_t)((fluffy.mood >> (7U - bit)) & 1U));
    }
    printf("\n");
    if (READ_BIT(fluffy.mood, BIT_HUNGRY)) {
        puts("Fluffy needs food.");
    } else if (READ_BIT(fluffy.mood, BIT_SAD)) {
        puts("Fluffy needs playtime.");
    } else if (READ_BIT(fluffy.mood, BIT_SICK)) {
        puts("Fluffy needs sleep to recover.");
    } else if (READ_BIT(fluffy.mood, BIT_ASLEEP)) {
        puts("Fluffy is resting.");
    } else if (isHappy()) {
        puts("Fluffy is happy and healthy.");
    } else {
        puts("Fluffy is doing fine.");
    }
}

int main(void)
{
    int choice = 0;

    hatchPet();
    do {
        printf("\n=== Fluffy the Robot Pet ===\n");
        drawFace();
        printf("1) Feed\n2) Play\n3) Sleep/Wake\n4) Pass an hour\n5) Report\n6) Hatch new pet\n7) Exit\nChoice: ");
        if (scanf("%d", &choice) != 1) {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) {
            }
            choice = 0;
            puts("Bad input. Try again.");
            continue;
        }
        switch (choice) {
        case 1:
            feed();
            break;
        case 2:
            play();
            break;
        case 3:
            sleepPet();
            break;
        case 4:
            hourPasses();
            break;
        case 5:
            petReport();
            break;
        case 6:
            hatchPet();
            puts("A fresh Fluffy arrives.");
            break;
        case 7:
            puts("Goodbye!");
            break;
        default:
            puts("Invalid choice.");
            break;
        }
    } while (choice != 7);

    return 0;
}
