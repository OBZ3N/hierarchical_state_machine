
#pragma once

struct RangeMapper
{
    RangeMapper(unsigned int input_range, unsigned int output_range)
        : m_input_index(0)
        , m_output_index(0)
        , m_error(0)
    {
        if (input_range < output_range)
        {
            m_stretched = true;
            m_range = output_range;
            m_div = (input_range ? input_range : 1);
            m_inc = (input_range ? 1 : 0);
        }
        else
        {
            m_stretched = false;
            m_range = input_range;
            m_div = (output_range ? output_range : 1);
            m_inc = (output_range ? 1 : 0);
        }
    }

    bool step(unsigned int& input_index, unsigned int& output_index, unsigned int& input_step, unsigned int& output_step)
    {
        if (m_stretched)
        {
            if (m_output_index == m_range)
                return false;

            input_index = m_input_index;
            output_index = m_output_index;
            input_step = m_inc;
            output_step = (m_range + m_error) / m_div;

            m_input_index += input_step;
            m_output_index += output_step;
            m_error += m_range - (m_div * output_step);
            return true;
        }
        else
        {
            if (m_input_index == m_range)
                return false;

            input_index = m_input_index;
            output_index = m_output_index;
            input_step = (m_range + m_error) / m_div;
            output_step = m_inc;

            m_input_index += input_step;
            m_output_index += output_step;
            m_error += m_range - (m_div * input_step);
            return true;
        }
    }

    void test()
    {
        utils::randomSeed(0);

        for (int i = 0; i < 1000; ++i)
        {
            unsigned int input_range = utils::random(0u, 100u);
            unsigned int output_range = utils::random(0u, 100u);
            unsigned int input_index, output_index, input_step, output_step;

            printf("size : (%u, %u)\n", input_range, output_range);

            RangeMapper range_mapper(input_range, output_range);

            while (range_mapper.step(input_index, output_index, input_step, output_step))
            {
                printf("(%u, %u) (%u, %u)\n", input_index, output_index, input_step, output_step);
            }
            printf("\n");
        }
    }
}