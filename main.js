while(1) {
    GPIO_write("GPIOC",13,0);
    sleep(500);
    GPIO_write("GPIOC",13,1);
    sleep(500);
}
var a = 1 + 1;
a;