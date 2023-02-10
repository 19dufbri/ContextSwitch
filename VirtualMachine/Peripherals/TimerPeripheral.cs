namespace VirtualMachine.Peripherals
{
    public class TimerPeripheral : IPeripheral
    {
        private int _timeScale = 0x1000;
        private int _timer = 0x1000;
        private bool _runTimer = false;
        private bool _isInterrupt = false;
        public byte? RunCycle()
        {
            if (!_isInterrupt && _runTimer)
                _timer--;
		        
            // If timer is zero, make interrupt
            if (_timer == 0) {
                _timer = _timeScale;
                _isInterrupt = true;
                return 0x00;
            }

            return null;
        }

        public void AcknowledgeInterrupt(byte number)
        {
            _isInterrupt = false;
        }

        public void DoWrite(ushort value)
        {
            switch (value & 0x8000)
            {
                case 0x0000:
                    // Control register
                    _runTimer = (value & 0x01) == 0x01;
                    break;
                case 0x8000:
                    // Timescale register
                    _timeScale = (value & 0x7FFF) << 1;
                    break;
            }
        }

        public ushort DoRead()
        {
            return 0x0000;
        }
    }
}