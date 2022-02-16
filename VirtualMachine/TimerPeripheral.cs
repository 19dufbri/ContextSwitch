namespace VirtualMachine
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

        public void DoWrite(byte addr, ushort value)
        {
            switch (addr)
            {
                case 0x00:
                    // Control register
                    _runTimer = (value & 0x01) == 0x01;
                    break;
                case 0x01:
                    // Timescale register
                    _timeScale = value;
                    break;
            }
        }

        public ushort DoRead(byte addr)
        {
            return 0x0000;
        }
    }
}