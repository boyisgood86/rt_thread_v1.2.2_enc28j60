#ifndef __W25_X_80_H_
#define __W25_X_80_H_

#include <spi.h>


#define SPI_FLASH_CS_LOW()       GPIO_ResetBits(GPIOB, GPIO_Pin_12)
#define SPI_FLASH_CS_HIGH()      GPIO_SetBits(GPIOB, GPIO_Pin_12)



struct stm32_spi_bus
{
    struct rt_spi_bus parent;
    SPI_TypeDef * SPI;

#ifdef SPI_USE_DMA
    DMA_Stream_TypeDef * DMA_Stream_TX;
    uint32_t DMA_Channel_TX;
    DMA_Stream_TypeDef * DMA_Stream_RX;
    uint32_t DMA_Channel_RX;
    uint32_t DMA_Channel_TX_FLAG_TC;
    uint32_t DMA_Channel_RX_FLAG_TC;
#endif /*SPI_USE_DMA*/ 
    
};


struct stm32_spi_cs
{
    GPIO_TypeDef * GPIOx;
    uint16_t GPIO_Pin;
};


  struct spi_flash_device
  {
      struct rt_device                flash_device;
      struct rt_device_blk_geometry   geometry;
      struct rt_spi_device *          rt_spi_device;
      struct rt_mutex                 lock;
  };

  extern rt_err_t w25x80_init(const char * flash_device_name,const char * spi_device_name);
  
#endif /*__W25_X_80_H_*/
