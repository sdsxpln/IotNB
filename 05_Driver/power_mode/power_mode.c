#include "power_mode.h"
#include "stdlib.h"
#include "lcd.h"


bool enter_VLPR()
{
	volatile uint32_t * regBase;
	uint8_t reg;
	SMC->PMPROT = 42;

	__disable_irq();
	__ISB();

	/* configure VLPR mode */
	SIM->CLKDIV1 = 0x10030000U;


	/* If need to update the FCRDIV. */
	if (0 != ((MCG->SC & MCG_SC_FCRDIV_MASK) >> MCG_SC_FCRDIV_SHIFT))
	{
		/* If fast IRC is in use currently, change to slow IRC. */
		if ((1 == ((MCG->S & MCG_S_IRCST_MASK) >> MCG_S_IRCST_SHIFT)) && ((((MCG->S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) == 1) || (MCG->C1 & MCG_C1_IRCLKEN_MASK)))
		{
			MCG->C2 = ((MCG->C2 & ~MCG_C2_IRCS_MASK) | (MCG_C2_IRCS(0)));
			while (((MCG->S & MCG_S_IRCST_MASK) >> MCG_S_IRCST_SHIFT) != 0)
			{
			}
		}
		/* Update FCRDIV. */
		MCG->SC = (MCG->SC & ~(MCG_SC_FCRDIV_MASK | MCG_SC_ATMF_MASK | MCG_SC_LOCS0_MASK)) | MCG_SC_FCRDIV(0);
	}

	/* Set internal reference clock selection. */
	MCG->C2 = (MCG->C2 & ~MCG_C2_IRCS_MASK) | (MCG_C2_IRCS(1));
	MCG->C1 = (MCG->C1 & ~(MCG_C1_IRCLKEN_MASK | MCG_C1_IREFSTEN_MASK)) | (uint8_t)2;

	/* If MCGIRCLK is used, need to wait for MCG_S_IRCST. */
	if ((((MCG->S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) == 1) || (2 & 2))
	{
		while (((MCG->S & MCG_S_IRCST_MASK) >> MCG_S_IRCST_SHIFT) != 1)
		{
		}
	}


	/* Disable low power */
	MCG->C2 &= ~MCG_C2_LP_MASK;

	MCG->C1 = ((MCG->C1 & ~MCG_C1_CLKS_MASK) | MCG_C1_CLKS(2));
	//    while (MCG_S_CLKST_VAL != 2)
	//    {
	//会过不去
	//    }

	/* Disable PLL. */
	MCG->C6 &= ~MCG_C6_PLLS_MASK;
	while (MCG->S & MCG_S_PLLST_MASK)
	{
	}

	uint8_t mcg_c4;
	bool change_drs = false;

	mcg_c4 = MCG->C4;

	MCG->C2 &= ~MCG_C2_LP_MASK; /* Disable lowpower. */

	/*
           Errata: ERR007993
           Workaround: Invert MCG_C4[DMX32] or change MCG_C4[DRST_DRS] before
           reference clock source changes, then reset to previous value after
           reference clock changes.
	 */
	if (0 ==  ((MCG->S & MCG_S_IREFST_MASK) >> MCG_S_IREFST_SHIFT))
	{
		change_drs = true;
		/* Change the LSB of DRST_DRS. */
		MCG->C4 ^= (1U << MCG_C4_DRST_DRS_SHIFT);
	}

	/* Set CLKS and IREFS. */
	MCG->C1 =
			((MCG->C1 & ~(MCG_C1_CLKS_MASK | MCG_C1_IREFS_MASK)) | (MCG_C1_CLKS(1)    /* CLKS = 1 */
					| MCG_C1_IREFS(1))); /* IREFS = 1 */
	/* Wait and check status. */
	while (1 !=  ((MCG->S & MCG_S_IREFST_MASK) >> MCG_S_IREFST_SHIFT))
	{
	}

	/* Errata: ERR007993 */
	if (change_drs)
	{
		MCG->C4 = mcg_c4;
	}

	while (1 != ((MCG->S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT))
	{
	}

	MCG->C4 = (mcg_c4 & ~(MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS_MASK)) | (MCG_C4_DMX32(0) | MCG_C4_DRST_DRS(0));

	MCG->C2 |= MCG_C2_LP_MASK;

	SIM->CLKDIV1 = 0x00040000U;
	SIM->SOPT2 = ((SIM->SOPT2 & ~SIM_SOPT2_PLLFLLSEL_MASK) | SIM_SOPT2_PLLFLLSEL(0));
	SIM->SOPT1 = ((SIM->SOPT1 & ~SIM_SOPT1_OSC32KSEL_MASK) | SIM_SOPT1_OSC32KSEL(3));

	reg = SMC->PMCTRL;
	/* configure VLPR mode */
	reg &= ~SMC_PMCTRL_RUNM_MASK;
	reg |= (2 << SMC_PMCTRL_RUNM_SHIFT);
	SMC->PMCTRL = reg;

	while (4 != SMC->PMSTAT)
	{
	}
	__enable_irq();
	__ISB();

	if ( (SMC->PMCTRL & SMC_PMCTRL_RUNM_MASK) == 0x40)
	{
		return true;
	}
	else
	{
		return false;
	}
}


bool enter_VLPS()
{
	volatile uint32_t * regBase;
	uint8_t reg;
	uint_8 i;

	//关闭LCD
	LCD_GCR_REG(LCD) &= (~LCD_GCR_LCDEN_MASK);//关闭LCD使能
	//设置MUX,关闭LCD
	for (i = 0; i < FP_NUM; i++)
	{
		*LCD_PORT[FP_Table[i]] &= ~PORT_PCR_MUX_MASK;
	}
	for (i = 0; i < BP_NUM; i++)
	{
		*LCD_PORT[BP_Table[i]] &= ~PORT_PCR_MUX_MASK;
	}



	PORTE_PCR0 = PORT_PCR_MUX(0x0);  	 //关闭UART1_TXD
	PORTE_PCR1 = PORT_PCR_MUX(0x0);   	 //关闭UART1_RXD

	PORTE_PCR16 = PORT_PCR_MUX(0x0); 	 //关闭UART2_TXD
	PORTE_PCR17 = PORT_PCR_MUX(0x0);	 //关闭UART2_RXD




	SMC->PMPROT = 42;

	__disable_irq();
	__ISB();


	//	vlps
	/* configure VLPS mode */
	reg = SMC->PMCTRL;
	reg &= ~SMC_PMCTRL_STOPM_MASK;
	reg |= (2 << SMC_PMCTRL_STOPM_SHIFT);
	SMC->PMCTRL = reg;

	/* Set the SLEEPDEEP bit to enable deep sleep mode */
	SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

	/* read back to make sure the configuration valid before enter stop mode */
	(void)SMC->PMCTRL;
	__DSB();
	__WFI();
	__ISB();

	/* check whether the power mode enter VLPS mode succeed */
	__enable_irq();
	__ISB();



	SMC->PMPROT = 42;

	reg = SMC->PMCTRL;
	/* configure Normal RUN mode */
	reg &= ~SMC_PMCTRL_RUNM_MASK;
	reg |= (0 << SMC_PMCTRL_RUNM_SHIFT);
	SMC->PMCTRL = reg;


	while (1 != SMC->PMSTAT)
	{
	}

	//开启LCD
	LCD_GCR_REG(LCD) |= LCD_GCR_LCDEN_MASK;//开启LCD使能
	//设置MUX,开启LCD
	for (i = 0; i < FP_NUM; i++)
	{
		*LCD_PORT[FP_Table[i]] &= ~PORT_PCR_MUX_MASK;
		*LCD_PORT[FP_Table[i]] |= PORT_PCR_MUX(7);
	}
	for (i = 0; i < BP_NUM; i++)
	{
		*LCD_PORT[BP_Table[i]] &= ~PORT_PCR_MUX_MASK;
		*LCD_PORT[BP_Table[i]] |= PORT_PCR_MUX(7);
	}


	PORTE_PCR0 = PORT_PCR_MUX(0x3);    //使能UART1_TXD
	PORTE_PCR1 = PORT_PCR_MUX(0x3);    //使能UART1_RXD

	PORTE_PCR16 = PORT_PCR_MUX(0x3);   //使能UART2_TXD
	PORTE_PCR17 = PORT_PCR_MUX(0x3);   //使能UART2_RXD


	if (SMC->PMCTRL & SMC_PMCTRL_STOPA_MASK)
	{
		return false;
	}
	else
	{
		return true;
	}


}

