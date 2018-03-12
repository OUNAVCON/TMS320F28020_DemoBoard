#include "clock.h"
#include <DSP28x_Project.h>
#include "clk.h"
#include "wdog.h"
#include "pll.h"

CLK_Handle myClk;

void init_clocks(){


    PLL_Handle myPll;
    WDOG_Handle myWDog;


    myClk = CLK_init((void *)CLK_BASE_ADDR, sizeof(CLK_Obj));
    myPll = PLL_init((void *)PLL_BASE_ADDR, sizeof(PLL_Obj));
    myWDog = WDOG_init((void *)WDOG_BASE_ADDR, sizeof(WDOG_Obj));
    WDOG_disable(myWDog);
    CLK_enableAdcClock(myClk);
    (*Device_cal)();


    //
    // Select the internal oscillator 1 as the clock source
    //
    CLK_setOscSrc(myClk, CLK_OscSrc_Internal);

    //
    // Setup the PLL for x10 /2 which will yield 40Mhz = 10Mhz * 8 / 2
    //
    PLL_setup(myPll, PLL_Multiplier_8, PLL_DivideSelect_ClkIn_by_2);
}
