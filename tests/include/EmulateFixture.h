#pragma once

#include <memory>
#include <vector>
#include <gtest/gtest.h>

#include <cassert>
#include "CPU.h"
#include "TestUtils.h"

extern std::unique_ptr<CPU> get_cpu(const std::string &test_name);

class EmulateFixture : public ::testing::Test {
public:
    EmulateFixture()
        : cpu(get_cpu(current_test_name()))
    {
        reset();
    }

    ~EmulateFixture()
    {
        cpu->write_coverage();
    }

    void reset()
    {
        cpu->reset();
        // Doesn't need to model the reset vector of FFF0:0000 otherwise we
        // need to handle wrapping around to 0.  Just start off away from the
        // interrupt vector table so we can easily detect interrupts.
        cpu->write_reg(IP, 0x1000);
        cpu->write_reg(SS, 0x2000);
        cpu->write_reg(ES, 0x4000);
        cpu->write_reg(DS, 0x6000);
    }

    void set_instruction(const std::vector<uint8_t> &instr)
    {
        for (size_t m = 0; m < instr.size(); ++m)
            cpu->write_mem8(get_phys_addr(cpu->read_reg(CS), cpu->read_reg(IP) + m),
                            instr[m]);
        instr_len = instr.size();
        // Force a prefetch fifo clear so we don't end up executing what was
        // there before we wrote this instruction.
        cpu->write_reg(IP, cpu->read_reg(IP));
    }

    void write_reg(GPR regnum, uint16_t val)
    {
        cpu->write_reg(regnum, val);
    }

    uint16_t read_reg(GPR regnum)
    {
        return cpu->read_reg(regnum);
    }

    void write_mem8(uint32_t addr, uint8_t val, GPR segment=DS)
    {
        cpu->write_mem8(get_phys_addr(cpu->read_reg(segment), addr), val);
    }
    void write_mem16(uint32_t addr, uint16_t val, GPR segment=DS)
    {
        cpu->write_mem16(get_phys_addr(cpu->read_reg(segment), addr), val);
    }
    void write_mem32(uint32_t addr, uint32_t val, GPR segment=DS)
    {
        cpu->write_mem32(get_phys_addr(cpu->read_reg(segment), addr), val);
    }

    void write_cstring(uint32_t addr, const char *str, GPR segment=DS)
    {
        do {
            write_mem8(addr++, *str++, segment);
        } while (*str);
        write_mem8(addr, 0, segment);
    }

    void write_vector8(uint32_t addr, std::vector<uint8_t> vec, GPR segment=DS)
    {
        for (auto v: vec) {
            write_mem8(addr, v, segment);
            ++addr;
        }
    }
    void write_vector16(uint32_t addr, std::vector<uint16_t> vec, GPR segment=DS)
    {
        for (auto v: vec) {
            write_mem16(addr, v, segment);
            addr += sizeof(uint16_t);
        }
    }

    std::string read_cstring(uint32_t addr, GPR segment=DS)
    {
        std::string str;

        char v;
        for (;;) {
            v = read_mem8(addr++, segment);
            if (!v)
                break;
            str += v;
        }

        return str;
    }

    uint8_t read_mem8(uint32_t addr, GPR segment=DS)
    {
        return cpu->read_mem8(get_phys_addr(cpu->read_reg(segment), addr));
    }
    uint16_t read_mem16(uint32_t addr, GPR segment=DS)
    {
        return cpu->read_mem16(get_phys_addr(cpu->read_reg(segment), addr));
    }
    uint32_t read_mem32(uint32_t addr, GPR segment=DS)
    {
        return cpu->read_mem32(get_phys_addr(cpu->read_reg(segment), addr));
    }

    void write_io8(uint32_t addr, uint8_t val)
    {
        cpu->write_io8(addr, val);
    }
    void write_io16(uint32_t addr, uint16_t val)
    {
        cpu->write_io16(addr, val);
    }
    void write_io32(uint32_t addr, uint32_t val)
    {
        cpu->write_io32(addr, val);
    }

    uint8_t read_io8(uint32_t addr)
    {
        return cpu->read_io8(addr);
    }
    uint16_t read_io16(uint32_t addr)
    {
        return cpu->read_io16(addr);
    }
    uint32_t read_io32(uint32_t addr)
    {
        return cpu->read_io32(addr);
    }

    void emulate(int count=1)
    {
        cpu->clear_side_effects();

        assert(count > 0);
        size_t len = 0;
        for (auto i = 0; i < count; ++i)
             len += cpu->step();
        ASSERT_EQ(len, instr_len);
    }

    void write_flags(uint16_t val)
    {
        cpu->write_flags(val);
    }

    uint16_t read_flags()
    {
        return cpu->read_flags();
    }

    bool instruction_had_side_effects()
    {
        return cpu->instruction_had_side_effects();
    }
protected:
    size_t instr_len;
    std::unique_ptr<CPU> cpu;
};
